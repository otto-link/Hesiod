/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/selector.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_select_rivers_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("talus_ref", "talus_ref", 0.1f, 0.01f, 10.f);
  node.add_attr<FloatAttribute>("clipping_ratio", "clipping_ratio", 50.f, 0.1f, 100.f);

  // attribute(s) order
  node.set_attr_ordered_key({"talus_ref", "clipping_ratio"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = true});
}

void compute_select_rivers_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // work on a single array as a temporary solution
    hmap::Array z_array = p_in->to_array();

    z_array = hmap::select_rivers(z_array,
                                  node.get_attr<FloatAttribute>("talus_ref"),
                                  node.get_attr<FloatAttribute>("clipping_ratio"));

    p_out->from_array_interp(z_array);

    // post-process
    post_process_heightmap(node, *p_out);
  }
}

} // namespace hesiod
