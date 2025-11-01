/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "attributes.hpp"

#include "highmap/interpolate_array.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"
#include "hesiod/model/nodes/receive_node.hpp"

using namespace attr;

namespace hesiod
{

void setup_receive_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)

  // nothing by default, this is going to be updated and populated
  // automatically by the graph manager
  std::vector<std::string> choices = {"NO TAG AVAILABLE"};
  ADD_ATTR(ChoiceAttribute, "tag", choices, choices.front());

  // adjust corresponding widget
  GET_REF("tag", ChoiceAttribute)->set_use_combo_list(true);
}

void compute_receive_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
  std::string      tag = GET("tag", ChoiceAttribute);

  // cast to specialized node
  ReceiveNode *p_receive_node = dynamic_cast<ReceiveNode *>(p_node);

  if (!p_receive_node)
  {
    Logger::log()->error("compute_receive_node: Failed to cast to ReceiveNode");
    return;
  }

  if (!p_receive_node->get_p_broadcast_params())
  {
    // at node construction, node may be computed without the graph
    // infos being set
    Logger::log()->trace("compute_receive_node: p_broadcast_params {}",
                         p_receive_node->get_p_broadcast_params() ? "ok" : "NOK");
    return;
  }

  if (p_receive_node->get_p_broadcast_params()->empty())
  {
    Logger::log()->trace("compute_receive_node: empty map");
    return;
  }

  if (p_receive_node->get_p_broadcast_params()->contains(tag))
  {
    BroadcastParam broadcast_param = p_receive_node->get_p_broadcast_params()->at(tag);

    // retrieve various pointers for this broadcast
    const hmap::CoordFrame *t_source = broadcast_param.t_source;
    const hmap::Heightmap  *p_h = broadcast_param.p_h;
    hmap::CoordFrame       *t_target = p_receive_node->get_p_coord_frame();

    if (t_source && p_h && t_target)
    {
      hmap::interpolate_heightmap(*p_h, *p_out, *t_source, *t_target);
    }
    else
    {
      Logger::log()->warn("broadcast inputs not available (nullptr)");
    }
  }
  else
  {
    Logger::log()->trace("tag {} not available in broadcast_param", tag);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
