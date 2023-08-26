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

void ViewTree::render_node_editor()
{
  ImNodes::EditorContextSet(this->p_node_editor_context);

  ImGui::Begin(("Node editor / " + this->id).c_str(),
               nullptr,
               ImGuiWindowFlags_MenuBar |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);

  // --- menu bar

  {
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
    if (ImGui::BeginMenuBar())
    {
      if (ImGui::BeginMenu("View"))
      {
        if (ImGui::MenuItem("Node list [N]",
                            nullptr,
                            this->open_node_list_window))
          this->open_node_list_window = !this->open_node_list_window;

        if (ImGui::MenuItem("Settings [S]", nullptr, this->show_settings))
          this->show_settings = !this->show_settings;

        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
    ImGui::PopItemWidth();

    // key bindings
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
    {
      if (ImGui::IsKeyReleased(ImGuiKey_N))
        this->open_node_list_window = !this->open_node_list_window;
      if (ImGui::IsKeyReleased(ImGuiKey_S))
        this->show_settings = !this->show_settings;
      if (ImGui::IsKeyReleased(ImGuiKey_1))
        this->open_view2d_window = !this->open_view2d_window;
    }
  }

  // --- node selection
  const int num_selected_nodes = ImNodes::NumSelectedNodes();
  this->selected_node_ids.clear();

  if (num_selected_nodes > 0)
  {
    this->selected_node_hash_ids.resize(num_selected_nodes);
    this->selected_node_ids.reserve(num_selected_nodes);
    ImNodes::GetSelectedNodes(this->selected_node_hash_ids.data());

    // convert hash ids to node ids
    for (auto &node_hash_id : this->selected_node_hash_ids)
    {
      std::string node_id = this->get_node_id_by_hash_id(node_hash_id);
      this->selected_node_ids.push_back(node_id);
    }
  }

  // --- settings
  if (this->show_settings)
  {
    ImGui::BeginChild("settings", ImVec2(256, 0), true);
    ImGui::TextUnformatted("Settings");
    for (auto &node_id : this->selected_node_ids)
    {
      ImGui::SeparatorText(node_id.c_str());
      this->render_settings(node_id);
    }
    ImGui::EndChild();
    ImGui::SameLine();
  }

  // --- editor canvas
  std::vector<std::string> node_id_to_remove = {};

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
      // set "dropped link" indicator to false when a node has been
      // created
      this->link_has_been_dropped = open_popup;

      // node selection management
      for (auto &node_id : this->selected_node_ids)
      {
        // duplicate node(s)
        if (ImGui::IsKeyReleased(ImGuiKey_D))
        {
          std::string new_node_id = this->add_view_node(
              this->get_node_ref_by_id(node_id)->get_node_type());
          // render the node before moving it
          this->render_view_node(new_node_id);

          ImVec2 pos = ImNodes::GetNodeScreenSpacePos(
              this->get_node_ref_by_id(node_id)->hash_id);
          ImNodes::SetNodeScreenSpacePos(
              this->get_node_ref_by_id(new_node_id)->hash_id,
              ImVec2(pos.x + 200.f, pos.y));
        }

        // delete node(s)
        if (ImGui::IsKeyReleased(ImGuiKey_Delete) or
            ImGui::IsKeyReleased(ImGuiKey_X))
          node_id_to_remove.push_back(node_id);

        // create a "Clone" node
        if (ImGui::IsKeyReleased(ImGuiKey_C))
        {
          std::string new_node_id = this->add_view_node("Clone");
          this->render_view_node(new_node_id);

          ImVec2 pos = ImNodes::GetNodeScreenSpacePos(
              this->get_node_ref_by_id(node_id)->hash_id);
          ImNodes::SetNodeScreenSpacePos(
              this->get_node_ref_by_id(new_node_id)->hash_id,
              ImVec2(pos.x + 200.f, pos.y));

          // automatic reconnection (only if there is an output named
          // "output")
          gnode::Node *p_node = this->get_node_ref_by_id(node_id);
          gnode::Node *p_clone = this->get_node_ref_by_id(new_node_id);

          if (p_node->is_port_id_in_keys("output"))
          {
            // hash id of the port on the other end of the link (if connected)
            int  port_hash_id_to = 0;
            bool previously_connected = false;
            if (p_node->get_port_ref_by_id("output")->is_connected)
            {
              port_hash_id_to =
                  p_node->get_port_ref_by_id("output")->p_linked_port->hash_id;
              previously_connected = true;
            }

            // selected node -> new Clone node
            this->new_link(p_node->get_port_ref_by_id("output")->hash_id,
                           p_clone->get_port_ref_by_id("input")->hash_id);

            // new Clone node -> previously connected input (if any)
            if (previously_connected)
              this->new_link(p_clone->get_port_ref_by_id("thru##0")->hash_id,
                             port_hash_id_to);
          }
        }
      }

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

  // insert Clone node
  if ((num_selected_links > 0) && ImGui::IsKeyReleased(ImGuiKey_C))
  {
    std::vector<int> selected_links;
    selected_links.resize(num_selected_links);
    ImNodes::GetSelectedLinks(selected_links.data());

    for (auto &link_id : selected_links)
    {
      // backup link infos
      Link link_bckp = *(this->get_link_ref_by_id(link_id));

      // place a new "Clone" in-between
      this->remove_link(link_id);
      std::string  new_node_id = this->add_view_node("Clone");
      gnode::Node *p_new_node = this->get_node_ref_by_id(new_node_id);

      this->new_link(link_bckp.port_hash_id_from,
                     p_new_node->get_port_ref_by_id("input")->hash_id);
      this->new_link(p_new_node->get_port_ref_by_id("thru##0")->hash_id,
                     link_bckp.port_hash_id_to);
      // TODO : move to the new node to the right position
    }
    ImNodes::ClearLinkSelection();
  }

  // remove the selected nodes for removal (has to be done AFTER the
  // links removal)
  for (auto &node_id : node_id_to_remove)
  {
    this->remove_view_node(node_id);
    ImNodes::ClearNodeSelection();
    this->selected_node_ids.clear();
  }

  ImGui::End();

  // --- 2D viewer
  if (this->open_view2d_window)
  {
    ImGui::Begin(("View 2D / " + this->id).c_str(), &this->open_view2d_window);
    if (this->selected_node_ids.size() > 0)
    {
      this->set_view2d_node_id(this->selected_node_ids.back());
      this->render_view2d(this->selected_node_ids.back());
    }
    ImGui::End();
  }

  // --- node list window
  if (this->open_node_list_window)
  {
    ImGui::Begin(("Node list / " + this->id).c_str(),
                 &this->open_node_list_window);

    ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter |
                            ImGuiTableFlags_BordersV;
    const int ncol = 7;

    if (ImGui::BeginTable(("table##" + this->id).c_str(), ncol, flags))
    {
      ImGui::TableSetupColumn("Hash Id");
      ImGui::TableSetupColumn("Node Id");
      ImGui::TableSetupColumn("Port Id");
      ImGui::TableSetupColumn("Port Direction");
      ImGui::TableSetupColumn("Link");
      ImGui::TableSetupColumn("Node Id");
      ImGui::TableSetupColumn("Port Id");
      ImGui::TableSetupScrollFreeze(0, 1);
      ImGui::TableHeadersRow();

      // separation
      ImGui::TableNextRow();
      for (int i = 0; i < ncol; i++)
      {
        ImGui::TableNextColumn();
        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                               IM_COL32(120, 120, 120, 255));
      }

      for (auto &[id, vnode] : this->get_nodes_map())
      {
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::TextColored(ImColor(IM_COL32(255, 255, 255, 100)),
                           "%d",
                           vnode.get()->hash_id);
        ImGui::TableNextColumn();
        ImGui::Text("%s", id.c_str());
        ImGui::TableNextColumn(); // empty (port id)
        ImGui::TableNextColumn(); // empty
        ImGui::TableNextColumn(); // empty
        ImGui::TableNextColumn(); // empty
        ImGui::TableNextColumn(); // empty

        // ports
        for (auto &[port_id, port] : vnode.get()->get_ports())
        {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::TextColored(ImColor(IM_COL32(255, 255, 255, 100)),
                             "%d",
                             port.hash_id);
          ImGui::TableNextColumn(); // empty (node id)
          ImGui::TableNextColumn();
          ImGui::Text("%s", port_id.c_str());

          ImGui::TableNextColumn();
          if (port.direction == gnode::direction::in)
            ImGui::TextUnformatted("Input");
          else
            ImGui::TextUnformatted("Output");

          ImGui::TableNextColumn();
          if (port.is_connected)
          {
            if (port.direction == gnode::direction::in)
              ImGui::TextUnformatted("<--");
            else
              ImGui::TextUnformatted("-->");
            ImGui::TableNextColumn();
            ImGui::Text("%s", port.p_linked_node->id.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", port.p_linked_port->id.c_str());
          }
          else
          {
            ImGui::TextUnformatted(" x");
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
          }
        }

        // separation
        ImGui::TableNextRow();
        for (int i = 0; i < ncol; i++)
        {
          ImGui::TableNextColumn();
          ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                                 IM_COL32(120, 120, 120, 255));
        }
      }

      ImGui::EndTable();
    }

    ImGui::End();
  }
}

void ViewTree::render_node_list()
{
  for (auto &[id, vnode] : this->get_nodes_map())
    ImGui::Text("id: %s, hash_id: %d", id.c_str(), vnode.get()->hash_id);
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
