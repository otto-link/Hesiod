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

std::string ViewTree::render_new_node_treeview(
    const hmap::Vec2<float> node_position)
{
  std::string new_node_id = "";
  const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
  const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

  static ImGuiTableFlags flags = ImGuiTableFlags_Reorderable |
                                 ImGuiTableFlags_Hideable |
                                 // ImGuiTableFlags_Sortable |
                                 // ImGuiTableFlags_SortMulti |
                                 // ImGuiTableFlags_RowBg |
                                 ImGuiTableFlags_BordersOuter |
                                 ImGuiTableFlags_BordersV |
                                 ImGuiTableFlags_NoBordersInBody |
                                 ImGuiTableFlags_ScrollY;

  if (ImGui::BeginTable("table_sorting",
                        2,
                        flags,
                        ImVec2(0.f, TEXT_BASE_HEIGHT * 10),
                        0.f))
  {
    ImGui::TableSetupColumn("Type",
                            ImGuiTableColumnFlags_DefaultSort |
                                ImGuiTableColumnFlags_WidthFixed,
                            0.f,
                            0);
    ImGui::TableSetupColumn("Category",
                            ImGuiTableColumnFlags_WidthFixed,
                            0.f,
                            1);
    ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
    ImGui::TableHeadersRow();

    ImGuiListClipper clipper;
    clipper.Begin((int)hesiod::cnode::category_mapping.size());

    while (clipper.Step())
    {
      int k = 0;
      for (auto &[node_type, node_category] : hesiod::cnode::category_mapping)
      {
        if ((k >= clipper.DisplayStart) & (k < clipper.DisplayEnd))
        {
          ImGui::PushID(k);
          ImGui::TableNextRow();
          ImGui::TableNextColumn();

          const bool item_is_selected = false;

          if (ImGui::Selectable(node_type.c_str(),
                                item_is_selected,
                                ImGuiSelectableFlags_SpanAllColumns))
          {
            LOG_DEBUG("selected node type: %s", node_type.c_str());
            new_node_id = this->add_view_node(node_type);

            // set node position: the node needs to be rendered first
            this->render_view_node(new_node_id);
            ImNodes::SetNodeScreenSpacePos(
                this->get_node_ref_by_id(new_node_id)->hash_id,
                ImVec2(node_position.x, node_position.y));
          }

          ImGui::TableNextColumn();

          std::string main_category = node_category.substr(
              0,
              node_category.find("/"));

          ImGui::TextUnformatted(node_category.c_str());
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                                 category_colors.at(main_category).hovered);
          ImGui::PopID();
        }
        k++;
      }
    }
    ImGui::EndTable();
  }

  return new_node_id;
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

void ViewTree::render_node_editor()
{
  ImNodes::EditorContextSet(this->p_node_editor_context);

  ImGui::Begin(("Node editor / " + this->id).c_str());

  // --- settings
  {
    ImGui::BeginChild("settings", ImVec2(256, 0), true);
    ImGui::TextUnformatted("Settings");

    const int num_selected_nodes = ImNodes::NumSelectedNodes();

    if (num_selected_nodes > 0)
    {
      std::vector<int> selected_nodes;
      selected_nodes.resize(num_selected_nodes);
      ImNodes::GetSelectedNodes(selected_nodes.data());

      std::vector<std::string> node_id_to_remove = {};

      for (auto &node_hash_id : selected_nodes)
      {
        std::string node_id = this->get_node_id_by_hash_id(node_hash_id);
        ImGui::SeparatorText(node_id.c_str());
        this->render_settings(node_id);

        // delete node
        if (ImGui::IsKeyReleased(ImGuiKey_Delete) or
            ImGui::IsKeyReleased(ImGuiKey_X))
          node_id_to_remove.push_back(node_id);

        // duplicate node
        // TODO : move node to mouse position after creation
        // const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
        if (ImGui::IsKeyReleased(ImGuiKey_D))
          this->add_view_node(
              this->get_node_ref_by_id(node_id)->get_node_type());
      }

      // TODO clean-up / keep view window open / show last node selected
      // int          node_hash_id = selected_nodes.back();
      // gnode::Node *p_node = this->get_node_ref_by_hash_id(node_hash_id);
      // ((hesiod::vnode::ViewControlNode *)p_node)->render_view2d("output");

      for (auto &node_id : node_id_to_remove)
      {
        this->remove_view_node(node_id);
        ImNodes::ClearNodeSelection();
      }
    }
    ImGui::EndChild();
    ImGui::SameLine();
  }

  // --- editor canvas
  {
    ImNodes::BeginNodeEditor();
    this->render_view_nodes();
    this->render_links();

    { // add node
      bool open_popup = ImGui::IsWindowFocused(
                            ImGuiFocusedFlags_RootAndChildWindows) &&
                        ImNodes::IsEditorHovered() &&
                        ImGui::IsMouseClicked(ImGuiMouseButton_Right);
      open_popup |= this->link_has_been_dropped;

      this->render_new_node_popup(open_popup);
      this->link_has_been_dropped = open_popup;
      // (set "dropped link" indicator to false when a node has
      // been created)

      // TODO : automatically connect to default in or out when a link
      // is dropped
    }

    ImNodes::EndNodeEditor();
  }

  // --- links management // new
  int port_hash_id_from, port_hash_id_to;
  if (ImNodes::IsLinkCreated(&port_hash_id_from, &port_hash_id_to))
    this->new_link(port_hash_id_from, port_hash_id_to);

  // --- links management // drop
  if (ImNodes::IsLinkDropped(&port_hash_id_from))
    // rendering the new node selection GUI is postponed because it
    // can only be done within the node editor context
    this->link_has_been_dropped = true;

  // --- links management // destruction
  const int num_selected_links = ImNodes::NumSelectedLinks();

  if ((num_selected_links > 0) & (ImGui::IsKeyReleased(ImGuiKey_Delete) or
                                  ImGui::IsKeyReleased(ImGuiKey_X)))
  {
    std::vector<int> selected_links;
    selected_links.resize(num_selected_links);
    ImNodes::GetSelectedLinks(selected_links.data());

    for (auto &v : selected_links)
      this->remove_link(v);
  }

  ImGui::End();
}

void ViewTree::render_node_list()
{
  for (auto &[id, vnode] : this->get_nodes_map())
    ImGui::Text("id: %s, hash_id: %d", id.c_str(), vnode.get()->hash_id);
}

void ViewTree::render_view_node(std::string node_id)
{
  hesiod::vnode::ViewControlNode *p_vnode =
      (hesiod::vnode::ViewControlNode *)(this->get_node_ref_by_id(node_id));
  p_vnode->render_node();
}

void ViewTree::render_view_nodes()
{
  for (auto &[id, node] : this->get_nodes_map())
  {
    hesiod::vnode::ViewControlNode *p_vnode =
        (hesiod::vnode::ViewControlNode *)(node.get());
    p_vnode->render_node();
  }
}

void ViewTree::render_settings(std::string node_id)
{
  hesiod::vnode::ViewControlNode *p_vnode =
      (hesiod::vnode::ViewControlNode *)this->get_node_ref_by_id(node_id);
  p_vnode->render_settings();
}

} // namespace hesiod::vnode
