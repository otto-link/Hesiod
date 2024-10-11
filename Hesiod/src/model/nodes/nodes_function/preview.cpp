/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_preview_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "elevation");
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::IN, "texture");
}

void compute_preview_node(BaseNode *p_node)
{
  LOG->trace("computing node {}", p_node->get_label());

  Q_EMIT p_node->compute_started(p_node->get_id()); // empty on purpose
  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
