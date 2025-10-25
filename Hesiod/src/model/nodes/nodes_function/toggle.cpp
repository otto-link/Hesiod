/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void setup_toggle_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input A");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input B");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(BoolAttribute, "toggle", "A", "B", true);

  // attribute(s) order
  p_node->set_attr_ordered_key({"toggle"});
}

void compute_toggle_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in_a = p_node->get_value_ref<hmap::Heightmap>("input A");
  hmap::Heightmap *p_in_b = p_node->get_value_ref<hmap::Heightmap>("input B");

  if (p_in_a && p_in_b)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the either A or B input heightmap based on the toggle state
    if (GET("toggle", BoolAttribute))
      *p_out = *p_in_a;
    else
      *p_out = *p_in_b;
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
