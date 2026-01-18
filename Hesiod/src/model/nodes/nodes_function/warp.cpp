/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/transform.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_warp_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "dx");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "dy");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("scaling.x", "scaling.x", 1.f, -2.f, 2.f);
  node.add_attr<FloatAttribute>("scaling.y", "scaling.y", 1.f, -2.f, 2.f);

  // attribute(s) order
  node.set_attr_ordered_key({"scaling.x", "scaling.y"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = false});
}

void compute_warp_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  if (p_in)
  {
    hmap::VirtualArray *p_dx = node.get_value_ref<hmap::VirtualArray>("dx");
    hmap::VirtualArray *p_dy = node.get_value_ref<hmap::VirtualArray>("dy");
    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");

    float sx = node.get_attr<FloatAttribute>("scaling.x");
    float sy = node.get_attr<FloatAttribute>("scaling.y");

    hmap::for_each_tile(
        {p_out, p_in, p_dx, p_dy},
        [sx, sy](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array dx = p_arrays[2] ? sx * (*p_arrays[1]) : hmap::Array(pa_out->shape);
          hmap::Array dy = p_arrays[3] ? sy * (*p_arrays[2]) : hmap::Array(pa_out->shape);

          *pa_out = *pa_in;

          hmap::gpu::warp(*pa_out, &dx, &dy);
        },
        node.cfg().cm_gpu);

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }
}

} // namespace hesiod
