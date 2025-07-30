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

void setup_cloud_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "background");
  p_node->add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  ADD_ATTR(CloudAttribute, "cloud");

  setup_background_image_for_cloud_attribute(p_node, "cloud", "background");
}

void compute_cloud_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Cloud *p_out = p_node->get_value_ref<hmap::Cloud>("cloud");
  *p_out = GET("cloud", CloudAttribute);

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
