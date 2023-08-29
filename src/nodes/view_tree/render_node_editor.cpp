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

        if (ImGui::MenuItem("View 2D [1]", nullptr, this->open_view2d_window))
          this->open_view2d_window = !this->open_view2d_window;

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

    if (ImGui::Button("Automatic layout"))
    {
      std::vector<gnode::Point> positions =
          this->compute_graph_layout_sugiyama();

      int k = 0;
      for (auto &[node_id, node] : this->get_nodes_map())
      {
        ImNodes::SetNodeScreenSpacePos(
            this->get_node_ref_by_id(node_id)->hash_id,
            ImVec2(512.f + 256.f * positions[k].x,
                   256.f + 256.f * positions[k].y));
        k++;
      }
    }
    ImGui::Separator();

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
    this->render_node_list();
    ImGui::End();
  }
}

} // namespace hesiod::vnode
