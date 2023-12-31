/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
#include "macrologger.h"
#include <imgui_node_editor.h>

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

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

void ViewTree::render_links()
{
  for (auto &[link_id, link] : this->links)
  {
    // change color of "dead" links when a node upstream is frozen
    bool is_link_frozen =
        this->get_node_ref_by_id(link.node_id_from)->frozen_outputs;

    int dtype = this->get_node_ref_by_id(link.node_id_from)
                    ->get_port_ref_by_id(link.port_id_from)
                    ->dtype;

    ImVec4 color = ImColor(dtype_colors.at(dtype).hovered);

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
  this->get_node_ref_by_id<ViewNode>(node_id)->render_node();
}

void ViewTree::render_view_nodes()
{
  for (auto &[id, node] : this->get_nodes_map())
    this->get_node_ref_by_id<ViewNode>(id)->render_node();
}

} // namespace hesiod::vnode
