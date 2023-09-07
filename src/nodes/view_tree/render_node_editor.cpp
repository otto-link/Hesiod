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

void ViewTree::render_node_editor()
{

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

        if (ImGui::MenuItem("View 2D [2]", nullptr, this->open_view2d_window))
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
      if (ImGui::IsKeyReleased(ImGuiKey_2))
        this->open_view2d_window = !this->open_view2d_window;
    }
  }

  // --- settings
  bool fit_to_content = false;
  bool fit_to_selection = false;
  bool automatic_layout = false;

  if (ImGui::Button("Fit to content"))
    fit_to_content = true;
  ImGui::SameLine();

  if (ImGui::Button("Fit to selection"))
    fit_to_selection = true;
  ImGui::SameLine();

  if (ImGui::Button("Automatic layout"))
    automatic_layout = true;

  if (this->show_settings)
  {
    ImGui::BeginChild("settings", ImVec2(256, 0), true);

    ImGui::TextUnformatted("Settings");
    for (auto &node_hid : selected_node_hid)
    {
      std::string node_id = this->get_node_ref_by_hash_id(node_hid.Get())->id;
      ImGui::SeparatorText(node_id.c_str());
      this->render_settings(node_id);
    }

    ImGui::EndChild();
    ImGui::SameLine();
  }

  // --- editor canvas

  ax::NodeEditor::SetCurrentEditor(this->p_node_editor_context);
  {
    ax::NodeEditor::Begin(id.c_str(), ImVec2(0.0, 0.0f));
    this->render_view_nodes();
    this->render_links();

    // --- panning
    if (fit_to_content)
      ax::NodeEditor::NavigateToContent(1);

    if (fit_to_selection)
      ax::NodeEditor::NavigateToSelection(true, 1);

    if (automatic_layout)
    {
      this->automatic_node_layout();
      ax::NodeEditor::NavigateToContent(1);
    }

    // --- creation
    if (ax::NodeEditor::BeginCreate())
    {
      // new link
      ax::NodeEditor::PinId port_hid_1;
      ax::NodeEditor::PinId port_hid_2;
      if (ax::NodeEditor::QueryNewLink(&port_hid_1, &port_hid_2))
        if (port_hid_1 && port_hid_2)
          if (ax::NodeEditor::AcceptNewItem())
            this->new_link(port_hid_1.Get(), port_hid_2.Get());

      if (ax::NodeEditor::QueryNewNode(&port_hid_1))
        if (ax::NodeEditor::AcceptNewItem())
        {
          ImGui::OpenPopup("add node");
          // TODO
          LOG_DEBUG("%d", port_hid_1.Get());
        }
    }
    ax::NodeEditor::EndCreate();

    // --- pop-up contextual menus
    {
      ax::NodeEditor::Suspend();

      // add node
      if (ax::NodeEditor::ShowBackgroundContextMenu())
        ImGui::OpenPopup("add node");
      this->render_new_node_popup();

      // settings
      if (ax::NodeEditor::ShowNodeContextMenu(&this->context_menu_node_hid))
        ImGui::OpenPopup("node settings");
      else
      {
        ax::NodeEditor::NodeId dclick_nid =
            ax::NodeEditor::GetDoubleClickedNode();
        if (dclick_nid)
        {
          this->context_menu_node_hid = dclick_nid;
          ImGui::OpenPopup("node settings");
        }
      }

      if (ImGui::BeginPopup("node settings"))
      {
        std::string node_id = this->get_node_id_by_hash_id(
            this->context_menu_node_hid.Get());
        ImGui::Text("%s", this->get_node_type(node_id).c_str());
        this->render_settings(node_id);
        ImGui::EndPopup();
      }

      ax::NodeEditor::Resume();
    }

    // --- deletion
    if (ax::NodeEditor::BeginDelete())
    {
      // link
      ax::NodeEditor::LinkId link_hid;
      if (ax::NodeEditor::QueryDeletedLink(&link_hid))
        if (ax::NodeEditor::AcceptDeletedItem())
          this->remove_link(link_hid.Get());

      // node
      ax::NodeEditor::NodeId node_hid;
      if (ax::NodeEditor::QueryDeletedNode(&node_hid))
        if (ax::NodeEditor::AcceptDeletedItem())
          this->remove_view_node(this->get_node_id_by_hash_id(node_hid.Get()));
    }
    ax::NodeEditor::EndDelete();

    // --- update selected nodes list
    this->selected_node_hid.resize(ax::NodeEditor::GetSelectedObjectCount());
    int nodeCount = ax::NodeEditor::GetSelectedNodes(
        this->selected_node_hid.data(),
        static_cast<int>(this->selected_node_hid.size()));
    this->selected_node_hid.resize(nodeCount);

    // --- create a "Clone" node
    if (this->selected_node_hid.size() && ImGui::IsKeyReleased(ImGuiKey_C))
      for (auto &node_hid : this->selected_node_hid)
      {
        std::string node_id = this->get_node_id_by_hash_id(node_hid.Get());
        this->insert_clone_node(node_id);
      }
  }

  ax::NodeEditor::End();
  ax::NodeEditor::SetCurrentEditor(nullptr);

  ImGui::End();

  // --- 2D viewer
  if (this->open_view2d_window)
  {
    ImGui::Begin(("View 2D / " + this->id).c_str(), &this->open_view2d_window);
    if (this->selected_node_hid.size() > 0)
    {
      std::string node_id = this->get_node_id_by_hash_id(
          this->selected_node_hid.back().Get());
      this->set_view2d_node_id(node_id);
      this->render_view2d(node_id);
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
