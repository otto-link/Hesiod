/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
#include "macrologger.h"
#include <imgui_node_editor.h>

#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

void ViewTree::render_links()
{
  for (auto &[link_id, link] : this->links)
  {
    // change color of "dead" links when a node upstream is frozen
    bool is_link_frozen =
        this->get_node_ref_by_id(link.node_id_from)->frozen_outputs;

    ImVec4 color = ImVec4(1.f, 1.f, 1.f, 1.f);
    if (is_link_frozen)
      color = ImVec4(0.6f, 0.2f, 0.2f, 1.f);

    ax::NodeEditor::Link(ax::NodeEditor::LinkId(link_id),
                         ax::NodeEditor::PinId(link.port_hash_id_from),
                         ax::NodeEditor::PinId(link.port_hash_id_to),
                         color);
  }
}

void ViewTree::render_view_node(std::string node_id)
{
  this->get_view_control_node_ref_by_id(node_id)->render_node();
}

void ViewTree::render_view_nodes()
{
  for (auto &[id, node] : this->get_nodes_map())
    this->render_view_node(id);
}

void ViewTree::render_view2d(std::string node_id)
{
  this->get_view_control_node_ref_by_id(node_id)->render_view2d();
}

void ViewTree::render_settings(std::string node_id)
{
  this->get_view_control_node_ref_by_id(node_id)->render_settings();
}

void ViewTree::automatic_node_layout()
{
  std::vector<gnode::Point> positions = this->compute_graph_layout_sugiyama();
  int                       k = 0;

  for (auto &[node_id, node] : this->get_nodes_map())
  {
    ax::NodeEditor::SetNodePosition(
        this->get_node_ref_by_id(node_id)->hash_id,
        ImVec2(512.f + 256.f * positions[k].x, 256.f + 256.f * positions[k].y));
    k++;
  }
}

} // namespace hesiod::vnode
