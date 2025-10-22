/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/selector.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_select_soil_weathered_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "radius_curvature", 0.f, 0.f, 0.1f);
  ADD_ATTR(FloatAttribute, "radius_gradient", 0.005f, 0.f, 0.1f);
  ADD_ATTR(FloatAttribute, "gradient_gain", 1.f, 0.01f, 10.f);
  ADD_ATTR(FloatAttribute, "curvature_weight", 1.f, -1.f, 1.f);
  ADD_ATTR(FloatAttribute, "gradient_weight", 0.2f, -1.f, 1.f);
  ADD_ATTR(EnumAttribute,
           "curvature_clamp_mode",
           clamping_mode_map,
           "Keep positive & clamp");
  ADD_ATTR(FloatAttribute, "curvature_clamping", 1.f, 0.f, FLT_MAX, "{:.4f}");

  // attribute(s) order
  p_node->set_attr_ordered_key({"_SEPARATOR_TEXT_Main parameters",
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
                                "gradient_gain"});

  setup_post_process_heightmap_attributes(p_node);
}

void compute_select_soil_weathered_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    int nx = p_out->shape.x; // for gradient scaling
    int ir_curv = (int)(GET("radius_curvature", FloatAttribute) * nx);
    int ir_grad = std::max(1, (int)(GET("radius_gradient", FloatAttribute) * nx));

    // --- compute gradient norm

    hmap::Heightmap grad_norm(CONFIG);

    hmap::transform(
        {&grad_norm, p_in},
        [p_node, ir_grad](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = hmap::gpu::morphological_gradient(*pa_in, ir_grad);
        },
        p_node->get_config_ref()->hmap_transform_mode_gpu);

    grad_norm.remap();

    // gain
    hmap::transform(
        {&grad_norm},
        [p_node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];

          hmap::gain(*pa_out, GET("gradient_gain", FloatAttribute));
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    // --- compute mask

    hmap::transform(
        {p_out, p_in, &grad_norm},
        [p_node, nx, ir_curv, ir_grad](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_grad_norm = p_arrays[2];

          auto mode = static_cast<hmap::ClampMode>(
              GET("curvature_clamp_mode", EnumAttribute));

          *pa_out = hmap::gpu::select_soil_weathered(
              *pa_in,
              *pa_grad_norm,
              ir_curv,
              mode,
              GET("curvature_clamping", FloatAttribute),
              GET("curvature_weight", FloatAttribute),
              GET("gradient_weight", FloatAttribute),
              (float)nx);
        },
        p_node->get_config_ref()->hmap_transform_mode_gpu);

    // post-process
    p_out->smooth_overlap_buffers();
    post_process_heightmap(p_node, *p_out);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
