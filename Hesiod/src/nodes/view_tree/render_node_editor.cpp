/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "ImGuiFileDialog.h"
#include "gnode.hpp"
#include "imgui.h"
#include "macrologger.h"
#include <imgui_node_editor.h>
#include <vector>

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"
#include "hesiod/widgets.hpp"

namespace hesiod::vnode
{

bool ViewTree::initialize_window()
{
  renderable_window_shortcuts.push_back(new shortcuts::GuiShortcut(
      "Preview 2d ##" + id,
      GLFW_KEY_2,
      0,
      [this](void *) { this->open_view2d_window = !this->open_view2d_window; },
      this->get_element_shortcut_group_id()));

  renderable_window_shortcuts.push_back(new shortcuts::GuiShortcut(
      "Preview 3d ##" + id,
      GLFW_KEY_3,
      0,
      [this](void *) { this->open_view3d_window = !this->open_view3d_window; },
      this->get_element_shortcut_group_id()));

  renderable_window_shortcuts.push_back(new shortcuts::GuiShortcut(
      "Settings ##" + id,
      GLFW_KEY_S,
      0,
      [this](void *) { this->show_settings = !this->show_settings; },
      this->get_element_shortcut_group_id()));

  return true;
}

bool ViewTree::render_element_content()
{
  this->render_node_editor();
  return true;
}

shortcuts::GuiShortcutGroupId ViewTree::get_element_shortcut_group_id()
{
  return "GroupViewTree" + this->id;
}

void ViewTree::render_node_editor()
{
  std::string title_bar = "Node editor ##" + this->id;

  bool fit_to_content = false;
  bool fit_to_selection = false;
  bool automatic_layout = false;

  // --- menu bar
  {
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
    if (ImGui::BeginMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.fileName = this->json_filename;

        if (ImGui::MenuItem("Load", "STRG O"))
          ImGuiFileDialog::Instance()->OpenDialog("LoadTreeStateDlg",
                                                  "Load Tree",
                                                  ".hsd",
                                                  config);

        bool save_button = ImGui::MenuItem("Save",
                                           "",
                                           false,
                                           this->json_filename.empty() ==
                                               false);
        bool save_as_button = ImGui::MenuItem("Save as", "");

        if ((save_button && this->json_filename == "") || save_as_button)
        {
          config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
          ImGuiFileDialog::Instance()->OpenDialog("SaveTreeStateDlg",
                                                  "Save Tree",
                                                  ".hsd",
                                                  config);
        }
        else if (save_button)
          this->save_state(this->json_filename);

        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Actions"))
      {
        if (ImGui::MenuItem("Fit to content"))
          fit_to_content = true;
        if (ImGui::MenuItem("Fit to selection"))
          fit_to_selection = true;
        if (ImGui::MenuItem("Automatic layout"))
          automatic_layout = true;
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View"))
      {
        if (ImGui::MenuItem("Node list", "N", this->open_node_list_window))
          this->open_node_list_window = !this->open_node_list_window;

        if (ImGui::MenuItem("Settings", "S", this->show_settings))
          this->show_settings = !this->show_settings;

        if (ImGui::MenuItem("View 2D", "2", this->open_view2d_window))
          this->open_view2d_window = !this->open_view2d_window;

        if (ImGui::MenuItem("View 3D", "3", this->open_view3d_window))
          this->open_view3d_window = !this->open_view3d_window;

        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
    ImGui::PopItemWidth();

    // key bindings
    // if (ImGui::IsWindowFocused())
    // {
    //   if (ImGui::IsKeyReleased(ImGuiKey_N))
    //     this->open_node_list_window = !this->open_node_list_window;
    //   if (ImGui::IsKeyReleased(ImGuiKey_S))
    //     this->show_settings = !this->show_settings;
    //   if (ImGui::IsKeyReleased(ImGuiKey_2))
    //     this->open_view2d_window = !this->open_view2d_window;
    //   if (ImGui::IsKeyReleased(ImGuiKey_3))
    //     this->open_view3d_window = !this->open_view3d_window;
    // }
  }

  if (this->show_settings)
  {
    ImGui::BeginChild("settings", ImVec2(320, 0), true);

    ImGui::TextUnformatted("Settings");
    for (auto &node_hid : selected_node_hid)
    {
      std::string node_id = this->get_node_ref_by_hash_id(node_hid.Get())->id;

      ImGui::Separator();
      hesiod::gui::draw_icon(hesiod::gui::square,
                             {12.f, 12.f},
                             this->get_node_color(node_id),
                             true);
      ImGui::SameLine();
      ImGui::Text("%s", this->get_node_type(node_id).c_str());

      this->render_settings(node_id);
      ImGui::Dummy(ImVec2(0.f, 16.f));
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
    ax::NodeEditor::PinId *p_port_hid_new = nullptr;
    ax::NodeEditor::PinId  port_hid_1;
    ax::NodeEditor::PinId  port_hid_2;

    if (ax::NodeEditor::BeginCreate())
    {
      // new link
      if (ax::NodeEditor::QueryNewLink(&port_hid_1, &port_hid_2))
        if (port_hid_1 && port_hid_2)
          if (ax::NodeEditor::AcceptNewItem())
            this->new_link(port_hid_1.Get(), port_hid_2.Get());

      if (ax::NodeEditor::QueryNewNode(&port_hid_1))
        if (ax::NodeEditor::AcceptNewItem())
        {
          ImGui::OpenPopup("add node");
          p_port_hid_new = &port_hid_1;
        }
      ax::NodeEditor::EndCreate();
    }

    // --- pop-up contextual menus
    {
      ax::NodeEditor::Suspend();

      // add node
      if (ax::NodeEditor::ShowBackgroundContextMenu())
        ImGui::OpenPopup("add node");
      std::string new_node_id = this->render_new_node_popup();

      if ((new_node_id != "") && p_port_hid_new)
      {
        // if the node has been created while dragging a new link,
        // automatically connect the link

        // TODO
      }

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

        hesiod::gui::draw_icon(hesiod::gui::square,
                               {12.f, 12.f},
                               this->get_node_color(node_id),
                               true);
        ImGui::SameLine();
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

  // --- 2D viewer
  if (this->open_view2d_window)
  {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, view3d_clear_color.Value);
    ImVec2 originalWindowPadding = ImGui::GetStyle().WindowPadding;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    if (ImGui::Begin(("View 2D ##" + this->id).c_str(),
                     &this->open_view2d_window,
                     ImGuiWindowFlags_MenuBar))
    {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, originalWindowPadding);
      if (this->selected_node_hid.size() > 0)
      {
        std::string node_id = this->get_node_id_by_hash_id(
            this->selected_node_hid.back().Get());
        this->set_viewer_node_id(node_id);
        this->render_view2d();
      }
      ImGui::PopStyleVar();
    }
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
  }

  // --- 3D viewer
  if (this->open_view3d_window)
  {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, view3d_clear_color.Value);
    ImVec2 originalWindowPadding = ImGui::GetStyle().WindowPadding;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    if (ImGui::Begin(("View 3D ##" + this->id).c_str(),
                     &this->open_view3d_window,
                     ImGuiWindowFlags_MenuBar))
    {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, originalWindowPadding);
      if (this->selected_node_hid.size() > 0)
      {
        std::string node_id = this->get_node_id_by_hash_id(
            this->selected_node_hid.back().Get());
        this->set_viewer_node_id(node_id);
        this->render_view3d();
      }
      ImGui::PopStyleVar();
    }
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
  }

  // --- node list window
  if (this->open_node_list_window)
  {
    if (ImGui::Begin(("Node list ##" + this->id).c_str(),
                     &this->open_node_list_window))
    {
      this->render_node_list();
    }
    ImGui::End();
  }

  // -- dialogs

  if (ImGuiFileDialog::Instance()->Display("LoadTreeStateDlg"))
  {
    if (ImGuiFileDialog::Instance()->IsOk())
    {
      std::string file_path_name =
          ImGuiFileDialog::Instance()->GetFilePathName();
      std::string file_path = ImGuiFileDialog::Instance()->GetCurrentPath();

      this->load_state(file_path_name);
      this->json_filename = file_path_name;
    }

    ImGuiFileDialog::Instance()->Close();
  }
  if (ImGuiFileDialog::Instance()->Display("SaveTreeStateDlg"))
  {
    if (ImGuiFileDialog::Instance()->IsOk())
    {
      std::string file_path_name =
          ImGuiFileDialog::Instance()->GetFilePathName();
      std::string file_path = ImGuiFileDialog::Instance()->GetCurrentPath();

      this->save_state(file_path_name);
      this->json_filename = file_path_name;
    }

    ImGuiFileDialog::Instance()->Close();
  }
}

} // namespace hesiod::vnode
