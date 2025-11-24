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

void setup_select_soil_flow_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("radius_gradient", "radius_gradient", 0.f, 0.f, 0.1f);
  node.add_attr<FloatAttribute>("gradient_weight", "gradient_weight", 1.f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("flow_weight", "flow_weight", 0.01f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("talus_ref", "talus_ref", 10.f, 0.01f, 32.f);
  node.add_attr<FloatAttribute>("clipping_ratio", "clipping_ratio", 50.f, 0.1f, 100.f);
  node.add_attr<FloatAttribute>("flow_gamma", "flow_gamma", 1.f, 0.01f, 4.f);

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Main Parameters",
                             "_TEXT_Weights & Blending",
                             "gradient_weight",
                             "flow_weight",
                             //
                             "_TEXT_Slope",
                             "radius_gradient",
                             //
                             "_TEXT_Rivers",
                             "talus_ref",
                             "clipping_ratio",
                             "flow_gamma",
                             "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(node);
}

void compute_select_soil_flow_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    int   nx = p_out->shape.x; // for gradient scaling
    int   ir = std::max(1, (int)(node.get_attr<FloatAttribute>("radius_gradient") * nx));
    float talus = node.get_attr<FloatAttribute>("talus_ref") / nx;

    // --- compute mask

    hmap::transform(
        {p_out, p_in},
        [&node, nx, ir, talus](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          float k_smooth = 0.01f; // little influence

          *pa_out = hmap::gpu::select_soil_flow(
              *pa_in,
              ir,
              node.get_attr<FloatAttribute>("gradient_weight"),
              (float)nx,
              node.get_attr<FloatAttribute>("flow_weight"),
              talus,
              node.get_attr<FloatAttribute>("clipping_ratio"),
              node.get_attr<FloatAttribute>("flow_gamma"),
              k_smooth);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    // post-process
    p_out->smooth_overlap_buffers();
    post_process_heightmap(node, *p_out);
  }
}

} // namespace hesiod
