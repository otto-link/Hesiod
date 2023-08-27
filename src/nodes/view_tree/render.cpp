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

  ImGuiTableFlags flags = ImGuiTableFlags_Reorderable |
                          ImGuiTableFlags_Hideable |
                          // ImGuiTableFlags_Sortable |
                          ImGuiTableFlags_BordersOuter |
                          ImGuiTableFlags_BordersV |
                          ImGuiTableFlags_NoBordersInBody |
                          ImGuiTableFlags_ScrollY;

  if (ImGui::BeginTable("table_sorting",
                        2,
                        flags,
                        ImVec2(0.f, TEXT_BASE_HEIGHT * 15),
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
          ImGui::TextColored(
              ImColor(category_colors.at(main_category).selected),
              node_category.c_str());
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
