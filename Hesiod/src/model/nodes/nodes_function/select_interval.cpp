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

void setup_select_interval_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("value1", "value1", 0.f, -0.5f, 1.5f);
  node.add_attr<FloatAttribute>("value2", "value2", 0.5f, -0.5f, 1.5f);

  // attribute(s) order
  node.set_attr_ordered_key({"value1", "value2"});

  setup_post_process_heightmap_attributes(node);
}

void compute_select_interval_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    hmap::transform(*p_out,
                    *p_in,
                    [&node](hmap::Array &out, hmap::Array &in)
                    {
                      out = hmap::select_interval(
                          in,
                          node.get_attr<FloatAttribute>("value1"),
                          node.get_attr<FloatAttribute>("value2"));
                    });

    // post-process
    post_process_heightmap(node, *p_out);
  }
}

} // namespace hesiod
