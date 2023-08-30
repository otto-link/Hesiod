/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
#include "imnodes.h"
#include "macrologger.h"

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

    if (is_link_frozen)
    {
      ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(150, 50, 50, 255));
      ImNodes::PushColorStyle(ImNodesCol_LinkHovered,
                              IM_COL32(200, 50, 50, 255));
    }

    ImNodes::Link(link_id, link.port_hash_id_from, link.port_hash_id_to);

    if (is_link_frozen)
    {
      ImNodes::PopColorStyle();
      ImNodes::PopColorStyle();
    }
  }
}

void ViewTree::render_new_node_popup(bool &open_popup)
{
  if (!ImGui::IsAnyItemHovered() && open_popup)
    ImGui::OpenPopup("add node");

  if (ImGui::BeginPopup("add node"))
  {
    const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

    if (this->render_new_node_treeview({click_pos.x, click_pos.y}) != "")
      ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
  }

  open_popup = false;
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

} // namespace hesiod::vnode
