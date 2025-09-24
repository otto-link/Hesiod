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

void setup_preview_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "water_depth");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "scalar");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "normal map");
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  p_node->add_port<hmap::Path>(gnode::PortType::IN, "path");
}

void compute_preview_node(BaseNode *p_node)
{
  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  Q_EMIT p_node->compute_started(p_node->get_id()); // empty on purpose
  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
