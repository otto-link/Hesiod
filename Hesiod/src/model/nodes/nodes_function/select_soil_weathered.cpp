/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/selector.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_select_soil_weathered_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("radius_curvature", "radius_curvature", 0.f, 0.f, 0.1f);
  node.add_attr<FloatAttribute>("radius_gradient", "radius_gradient", 0.005f, 0.f, 0.1f);
  node.add_attr<FloatAttribute>("gradient_gain", "gradient_gain", 1.f, 0.01f, 10.f);
  node.add_attr<FloatAttribute>("curvature_weight", "curvature_weight", 1.f, -1.f, 1.f);
  node.add_attr<FloatAttribute>("gradient_weight", "gradient_weight", 0.2f, -1.f, 1.f);
  node.add_attr<EnumAttribute>("curvature_clamp_mode",
                               "curvature_clamp_mode",
                               enum_mappings.clamping_mode_map,
                               "Keep positive & clamp");
  node.add_attr<FloatAttribute>("curvature_clamping",
                                "curvature_clamping",
                                1.f,
                                0.f,
                                FLT_MAX,
                                "{:.4f}");

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Main Parameters",
                             "_TEXT_Weights",
                             "curvature_weight",
                             "gradient_weight",
                             //
                             "_TEXT_Curvature",
                             "radius_curvature",
                             "curvature_clamp_mode",
                             "curvature_clamping",
                             //
                             "_TEXT_Slope",
                             "radius_gradient",
                             "gradient_gain",
                             "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(node);
}

void compute_select_soil_weathered_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int nx = p_out->shape.x; // for gradient scaling
    int ir_curv = (int)(node.get_attr<FloatAttribute>("radius_curvature") * nx);
    int ir_grad = std::max(1,
                           (int)(node.get_attr<FloatAttribute>("radius_gradient") * nx));

    // --- compute gradient norm

    hmap::Heightmap grad_norm(CONFIG(node));

    hmap::transform(
        {&grad_norm, p_in},
        [&node, ir_grad](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = hmap::gpu::morphological_gradient(*pa_in, ir_grad);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    grad_norm.remap();

    // gain
    hmap::transform(
        {&grad_norm},
        [&node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];

          hmap::gain(*pa_out, node.get_attr<FloatAttribute>("gradient_gain"));
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    // --- compute mask

    hmap::transform(
        {p_out, p_in, &grad_norm},
        [&node, nx, ir_curv, ir_grad](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_grad_norm = p_arrays[2];

          auto mode = static_cast<hmap::ClampMode>(
              node.get_attr<EnumAttribute>("curvature_clamp_mode"));

          *pa_out = hmap::gpu::select_soil_weathered(
              *pa_in,
              *pa_grad_norm,
              ir_curv,
              mode,
              node.get_attr<FloatAttribute>("curvature_clamping"),
              node.get_attr<FloatAttribute>("curvature_weight"),
              node.get_attr<FloatAttribute>("gradient_weight"),
              (float)nx);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    // post-process
    p_out->smooth_overlap_buffers();
    post_process_heightmap(node, *p_out);
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
