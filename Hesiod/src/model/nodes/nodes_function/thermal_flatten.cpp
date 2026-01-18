/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_thermal_flatten_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("talus_global", "talus_global", 1.f, 0.f, FLT_MAX);
  node.add_attr<IntAttribute>("iterations", "iterations", 100, 1, INT_MAX);
  node.add_attr<BoolAttribute>("scale_talus_with_elevation",
                               "scale_talus_with_elevation",
                               false);
  node.add_attr<FloatAttribute>("post_filter_radius",
                                "post_filter_radius",
                                0.01f,
                                0.f,
                                0.1f);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"talus_global", "iterations", "scale_talus_with_elevation", "post_filter_radius"});
}

void compute_thermal_flatten_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  if (p_in)
  {
    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");

    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;

    hmap::VirtualArray talus_map = hmap::VirtualArray(CONFIG(node));
    talus_map.fill(talus, node.cfg().cm_cpu);

    if (node.get_attr<BoolAttribute>("scale_talus_with_elevation"))
    {
      talus_map.copy_from(*p_in, node.cfg().cm_cpu);
      talus_map.remap(talus / 100.f, talus, node.cfg().cm_cpu);
    }

    int ir = std::max(
        1,
        (int)(node.get_attr<FloatAttribute>("post_filter_radius") * p_out->shape.x));

    hmap::for_each_tile(
        {p_out, p_in, &talus_map},
        [&node, talus, ir](std::vector<hmap::Array *> p_arrays,
                           const hmap::TileRegion    &region)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_talus_map = p_arrays[2];

          hmap::Array bedrock(region.shape, -std::numeric_limits<float>::max());

          *pa_out = *pa_in;

          hmap::thermal_flatten(*pa_out,
                                *pa_talus_map,
                                bedrock,
                                node.get_attr<IntAttribute>("iterations"),
                                ir);
        },
        node.cfg().cm_cpu);

    p_out->smooth_overlap_buffers();
  }
}

} // namespace hesiod
