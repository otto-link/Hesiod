/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "attributes.hpp"

#include "hesiod/graph_editor.hpp"
#include "hesiod/graph_manager.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/receive_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_receive_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)

  // nothing by default, this is going to be updated and populated
  // automatically by the graph manager
  std::vector<std::string> choices = {"NO TAG AVAILABLE"};
  ADD_ATTR(ChoiceAttribute, "tag", choices, choices.front());
}

void compute_receive_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
  std::string      tag = GET("tag", ChoiceAttribute);

  // cast to specialized node
  ReceiveNode *p_receive_node = dynamic_cast<ReceiveNode *>(p_node);

  if (!p_receive_node)
  {
    LOG->error("compute_receive_node: Failed to cast to ReceiveNode");
    return;
  }

  if (!p_receive_node->get_p_broadcast_params())
  {
    // at node construction, node may be computed without the graph
    // infos being set
    LOG->trace("compute_receive_node: p_broadcast_params {}",
               p_receive_node->get_p_broadcast_params() ? "ok" : "NOK");
    return;
  }

  if (p_receive_node->get_p_broadcast_params()->empty())
  {
    LOG->trace("compute_receive_node: empty map");
    return;
  }

  if (p_receive_node->get_p_broadcast_params()->contains(tag))
  {
    BroadcastParam broadcast_param = p_receive_node->get_p_broadcast_params()->at(tag);

    // retrieve various pointers for this broadcast
    const hmap::Terrain   *t_source = broadcast_param.t_source;
    const hmap::Heightmap *p_h = broadcast_param.p_h;
    hmap::Terrain         *t_target = p_receive_node->get_p_target_terrain();

    if (t_source && p_h && t_target)
    {
      hmap::Vec2<float> size_s = t_source->get_size();
      hmap::Vec2<float> size_t = t_target->get_size();

      hmap::Vec4<float> bbox_s = {0.f, size_s.x, 0.f, size_s.y};
      hmap::Vec4<float> bbox_t = {0.f, size_t.x, 0.f, size_t.y};

      // work on a copy of the incoming heightmap
      hmap::Heightmap h_src = *p_h;

      h_src.set_bbox(bbox_s);
      p_out->set_bbox(bbox_t);

      hmap::interpolate_terrain_heightmap(*t_source, h_src, *t_target, *p_out);
    }
    else
    {
      LOG->warn("broadcast inputs not available (nullptr)");
    }
  }
  else
  {
    LOG->trace("tag {} not available in broadcast_param", tag);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
