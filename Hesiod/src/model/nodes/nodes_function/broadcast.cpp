/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "attributes.hpp"

#include "hesiod/graph_editor.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/broadcast_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_broadcast_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "thru", CONFIG);

  // attribute(s)

  bool read_only = true; // tag is indeed set automatically
  ADD_ATTR(StringAttribute, "tag", "UNDEFINED", read_only);
}

void compute_broadcast_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_thru = p_node->get_value_ref<hmap::Heightmap>("thru");
    *p_thru = *p_in;

    BroadcastNode *p_broadcast_node = dynamic_cast<BroadcastNode *>(p_node);
    std::string    broadcast_tag = p_broadcast_node->get_broadcast_tag();

    std::string graph_id = p_node->get_graph_id();
    std::string node_id = p_node->get_id();

    LOG->trace("compute_broadcast_node: broadcasting graph: {}, node: {}",
               graph_id,
               node_id);

    LOG->trace("broadcast_tag: {}", broadcast_tag);

    // this goes to the graph editor
    Q_EMIT p_broadcast_node->broadcast_node_updated(graph_id, broadcast_tag);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
