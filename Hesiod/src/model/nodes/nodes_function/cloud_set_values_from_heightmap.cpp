/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_set_values_from_heightmap_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "heightmap");
  p_node->add_port<hmap::Cloud>(gnode::PortType::OUT, "out");

  // attribute(s)

  // attribute(s) order
}

void compute_cloud_set_values_from_heightmap_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Cloud     *p_cloud = p_node->get_value_ref<hmap::Cloud>("cloud");
  hmap::Heightmap *p_hmap = p_node->get_value_ref<hmap::Heightmap>("heightmap");

  if (p_cloud && p_hmap)
  {
    hmap::Cloud *p_out = p_node->get_value_ref<hmap::Cloud>("out");

    // TODO distribute
    hmap::Array array = p_hmap->to_array();

    *p_out = *p_cloud;
    p_out->set_values_from_array(array);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
