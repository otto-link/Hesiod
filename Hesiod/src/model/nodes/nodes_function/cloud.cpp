/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/geometry/cloud.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "background");
  node.add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  node.add_attr<CloudAttribute>("cloud", "cloud");

  setup_background_image_for_cloud_attribute(node, "cloud", "background");
}

void compute_cloud_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Cloud *p_out = node.get_value_ref<hmap::Cloud>("cloud");
  *p_out = node.get_attr<CloudAttribute>("cloud");

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
