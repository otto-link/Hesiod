/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void setup_toggle_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input A");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input B");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<BoolAttribute>("toggle", "toggle", "A", "B", true);

  // attribute(s) order
  node.set_attr_ordered_key({"toggle"});
}

void compute_toggle_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in_a = node.get_value_ref<hmap::Heightmap>("input A");
  hmap::Heightmap *p_in_b = node.get_value_ref<hmap::Heightmap>("input B");

  if (p_in_a && p_in_b)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the either A or B input heightmap based on the toggle state
    if (node.get_attr<BoolAttribute>("toggle"))
      *p_out = *p_in_a;
    else
      *p_out = *p_in_b;
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
