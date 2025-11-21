/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/blending.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_blend_poisson_bf_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 1");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input 2");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<IntAttribute>("iterations", "iterations", 500, 1, INT_MAX);

  node.add_attr<BoolAttribute>("inverse", "inverse", false);

  node.add_attr<RangeAttribute>("remap", "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"iterations", "_SEPARATOR_", "inverse", "remap"});
}

void compute_blend_poisson_bf_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in1 = node.get_value_ref<hmap::Heightmap>("input 1");
  hmap::Heightmap *p_in2 = node.get_value_ref<hmap::Heightmap>("input 2");

  if (p_in1 && p_in2)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");

    hmap::transform(
        {p_out, p_in1, p_in2, p_mask},
        [&node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in1 = p_arrays[1];
          hmap::Array *pa_in2 = p_arrays[2];
          hmap::Array *pa_mask = p_arrays[3];

          *pa_out = hmap::gpu::blend_poisson_bf(*pa_in1,
                                                *pa_in2,
                                                node.get_attr<IntAttribute>("iterations"),
                                                pa_mask);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(node,
                           *p_out,
                           node.get_attr<BoolAttribute>("inverse"),
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           node.get_attr_ref<RangeAttribute>("remap")->get_is_active(),
                           node.get_attr<RangeAttribute>("remap"));
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
