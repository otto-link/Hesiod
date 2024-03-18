/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"
#include "hesiod/widgets.hpp"
#include "hesiod/windows.hpp"

namespace hesiod::gui
{

void NodeEditor::automatic_node_layout()
{
  std::vector<gnode::Point> positions =
      this->p_vtree->compute_graph_layout_sugiyama();
  int k = 0;

  for (auto &[node_id, node] : this->p_vtree->get_nodes_map())
  {
    ax::NodeEditor::SetNodePosition(
        this->p_vtree->get_node_ref_by_id(node_id)->hash_id,
        ImVec2(512.f + 256.f * positions[k].x, 256.f + 256.f * positions[k].y));
    k++;
  }
}

ax::NodeEditor::EditorContext *NodeEditor::get_p_node_editor_context() const
{
  return this->p_node_editor_context;
}

ShortcutGroupId NodeEditor::get_element_shortcut_group_id()
{
  return "GroupNodeEditor" + this->get_unique_id();
}

bool NodeEditor::initialize()
{
  LOG_DEBUG("initializing NodeEditor window [%s] for ViewTree [%s]",
            this->get_unique_id().c_str(),
            this->p_vtree->id.c_str());

  this->title = "Node editor " + this->p_vtree->id;
  this->flags = ImGuiWindowFlags_MenuBar;

  // initialize node editor
  ax::NodeEditor::Config config;
  config.NavigateButtonIndex = 2;
  config.ContextMenuButtonIndex = 1;

  this->p_node_editor_context = ax::NodeEditor::CreateEditor(&config);

  // setup shortcuts

  this->shortcuts.push_back(std::make_unique<gui::Shortcut>(
      "Preview 2d ##" + this->get_unique_id(),
      GLFW_KEY_2,
      0,
      [this]()
      {
        this->get_window_manager_ref()->add_window(
            std::make_unique<hesiod::gui::Viewer2D>(this->p_vtree));
      },
      this->get_element_shortcut_group_id()));

  this->shortcuts.push_back(std::make_unique<gui::Shortcut>(
      "Preview 3d ##" + this->get_unique_id(),
      GLFW_KEY_3,
      0,
      [this]()
      {
        this->get_window_manager_ref()->add_window(
            std::make_unique<hesiod::gui::Viewer3D>(this->p_vtree));
      },
      this->get_element_shortcut_group_id()));

  this->shortcuts.push_back(std::make_unique<gui::Shortcut>(
      "Settings ##" + this->get_unique_id(),
      GLFW_KEY_S,
      0,
      [this]()
      {
        this->get_window_manager_ref()->add_window(
            std::make_unique<hesiod::gui::NodeSettings>(this->p_vtree));
      },
      this->get_element_shortcut_group_id()));

  return true;
}

void NodeEditor::insert_clone_node(std::string node_id)
{
  std::string new_node_id = this->p_vtree->add_view_node("Clone");
  this->render_view_node(new_node_id);

  ImVec2 pos = ax::NodeEditor::GetNodePosition(
      this->p_vtree->get_node_ref_by_id(node_id)->hash_id);
  ax::NodeEditor::SetNodePosition(
      this->p_vtree->get_node_ref_by_id(new_node_id)->hash_id,
      ImVec2(pos.x + 200.f, pos.y));

  // automatic reconnection (only if there is an output named
  // "output")
  gnode::Node *p_node = this->p_vtree->get_node_ref_by_id(node_id);
  gnode::Node *p_clone = this->p_vtree->get_node_ref_by_id(new_node_id);

  if (p_node->is_port_id_in_keys("output"))
  {
    // hash id of the port on the other end of the link (if
    // connected)
    int port_hash_id_to = 0;

    bool previously_connected = false;
    if (p_node->get_port_ref_by_id("output")->is_connected)
    {
      port_hash_id_to =
          p_node->get_port_ref_by_id("output")->p_linked_port->hash_id;
      previously_connected = true;
    }

    // selected node -> new Clone node
    this->p_vtree->new_link(p_node->get_port_ref_by_id("output")->hash_id,
                            p_clone->get_port_ref_by_id("input")->hash_id);

    // new Clone node -> previously connected input (if any)
    if (previously_connected)
      this->p_vtree->new_link(p_clone->get_port_ref_by_id("thru##0")->hash_id,
                              port_hash_id_to);
  }
}

bool NodeEditor::render_content()
{
  bool fit_to_content = false;
  bool fit_to_selection = false;
  bool automatic_layout = false;

  // --- menu bar

  {
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
    if (ImGui::BeginMenuBar())
    {
      // if (ImGui::BeginMenu("File"))
      // {
      //   IGFD::FileDialogConfig config;
      //   config.path = ".";
      //   config.fileName = this->p_vtree->json_filename;

      //   if (ImGui::MenuItem("Load", ""))
      //     ImGuiFileDialog::Instance()->OpenDialog("LoadTreeStateDlg",
      //                                             "Load Tree",
      //                                             ".hsd",
      //                                             config);

      //   bool save_button = ImGui::MenuItem("Save",
      //                                      "",
      //                                      false,
      //                                      this->p_vtree->json_filename.empty()
      //                                      ==
      //                                          false);
      //   bool save_as_button = ImGui::MenuItem("Save as", "");

      //   if ((save_button && this->p_vtree->json_filename == "") ||
      //   save_as_button)
      //   {
      //     config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
      //     ImGuiFileDialog::Instance()->OpenDialog("SaveTreeStateDlg",
      //                                             "Save Tree",
      //                                             ".hsd",
      //                                             config);
      //   }
      //   else if (save_button)
      //     this->p_vtree->save_state(this->p_vtree->json_filename);

      //   ImGui::EndMenu();
      // }

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
        // if (ImGui::MenuItem("Node list", "N",
        // this->p_vtree->open_node_list_window))
        //   this->p_vtree->open_node_list_window =
        //   !this->p_vtree->open_node_list_window;

        // if (ImGui::MenuItem("Node settings", "S", this->show_nodes_settings))
        //   this->show_nodes_settings = !this->show_nodes_settings;

        // if (ImGui::MenuItem("View 2D", "2",
        // this->p_vtree->open_view2d_window))
        //   this->p_vtree->open_view2d_window =
        //   !this->p_vtree->open_view2d_window;

        // if (ImGui::MenuItem("View 3D", "3",
        // this->p_vtree->open_view3d_window))
        //   this->p_vtree->open_view3d_window =
        //   !this->p_vtree->open_view3d_window;

        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
    ImGui::PopItemWidth();
  }

  if (this->show_nodes_settings)
  {
    ImGui::BeginChild("settings", ImVec2(320, 0), true);

    ImGui::TextUnformatted("Settings");
    for (auto &node_hid : selected_node_hid)
    {
      std::string node_id =
          this->p_vtree->get_node_ref_by_hash_id(node_hid.Get())->id;

      ImGui::Separator();
      hesiod::gui::draw_icon(hesiod::gui::square,
                             {12.f, 12.f},
                             this->p_vtree->get_node_color(node_id),
                             true);
      ImGui::SameLine();
      ImGui::Text("%s", this->p_vtree->get_node_type(node_id).c_str());

      this->p_vtree->render_settings(node_id);
      ImGui::Dummy(ImVec2(0.f, 16.f));
    }

    ImGui::EndChild();
    ImGui::SameLine();
  }

  // --- editor canvas

  ax::NodeEditor::SetCurrentEditor(this->p_node_editor_context);
  {
    ax::NodeEditor::Begin(this->p_vtree->id.c_str(), ImVec2(0.0, 0.0f));
    this->render_view_nodes();
    this->render_links();

    // reload node positions (they might have been moved by some other
    // process...
    for (auto &[node_id, pos] : this->p_vtree->get_node_positions())
      ax::NodeEditor::SetNodePosition(
          this->p_vtree->get_node_ref_by_id(node_id)->hash_id,
          pos);

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
            this->p_vtree->new_link(port_hid_1.Get(), port_hid_2.Get());

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
        std::string node_id = this->p_vtree->get_node_id_by_hash_id(
            this->context_menu_node_hid.Get());

        hesiod::gui::draw_icon(hesiod::gui::square,
                               {12.f, 12.f},
                               this->p_vtree->get_node_color(node_id),
                               true);
        ImGui::SameLine();
        ImGui::Text("%s", this->p_vtree->get_node_type(node_id).c_str());

        this->p_vtree->render_settings(node_id);
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
          this->p_vtree->remove_link(link_hid.Get());

      // node
      ax::NodeEditor::NodeId node_hid;
      if (ax::NodeEditor::QueryDeletedNode(&node_hid))
        if (ax::NodeEditor::AcceptDeletedItem())
          this->p_vtree->remove_view_node(
              this->p_vtree->get_node_id_by_hash_id(node_hid.Get()));
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
        std::string node_id = this->p_vtree->get_node_id_by_hash_id(
            node_hid.Get());
        this->insert_clone_node(node_id);
      }
  }

  ax::NodeEditor::End();
  ax::NodeEditor::SetCurrentEditor(nullptr);

  // --- keep a copy of some editor data within the ViewTree instance for
  // --- conveniency
  this->p_vtree->set_selected_node_hid(this->selected_node_hid);
  this->p_vtree->store_node_positions(this->get_p_node_editor_context());

  // --- 2D viewer

  // if (this->p_vtree->open_view2d_window)
  // {
  //   if (ImGui::Begin(("View 2D ##" + this->p_vtree->id).c_str(),
  //                    &this->p_vtree->open_view2d_window))
  //   {
  //     if (this->selected_node_hid.size() > 0)
  //     {
  //       std::string node_id = this->p_vtree->get_node_id_by_hash_id(
  //           this->selected_node_hid.back().Get());
  //       this->p_vtree->set_viewer_node_id(node_id);
  //       this->p_vtree->render_view2d();
  //     }
  //   }
  //   ImGui::End();
  // }

  // --- 3D viewer

  // if (this->p_vtree->open_view3d_window)
  // {
  //   if (ImGui::Begin(("View 3D ##" + this->p_vtree->id).c_str(),
  //                    &this->p_vtree->open_view3d_window))
  //   {
  //     if (this->selected_node_hid.size() > 0)
  //     {
  //       std::string node_id = this->p_vtree->get_node_id_by_hash_id(
  //           this->selected_node_hid.back().Get());
  //       this->p_vtree->set_viewer_node_id(node_id);
  //       this->p_vtree->render_view3d();
  //     }
  //   }
  //   ImGui::End();
  // }

  // --- node list window

  // if (this->p_vtree->open_node_list_window)
  // {
  //   if (ImGui::Begin(("Node list ##" + this->p_vtree->id).c_str(),
  //                    &this->p_vtree->open_node_list_window))
  //   {
  //     this->p_vtree->render_node_list();
  //   }
  //   ImGui::End();
  // }

  // -- dialogs

  // if (ImGuiFileDialog::Instance()->Display("LoadTreeStateDlg"))
  // {
  //   if (ImGuiFileDialog::Instance()->IsOk())
  //   {
  //     std::string file_path_name =
  //         ImGuiFileDialog::Instance()->GetFilePathName();
  //     std::string file_path = ImGuiFileDialog::Instance()->GetCurrentPath();

  //     this->p_vtree->load_state(file_path_name);
  //     this->p_vtree->json_filename = file_path_name;
  //   }

  //   ImGuiFileDialog::Instance()->Close();
  // }
  // if (ImGuiFileDialog::Instance()->Display("SaveTreeStateDlg"))
  // {
  //   if (ImGuiFileDialog::Instance()->IsOk())
  //   {
  //     std::string file_path_name =
  //         ImGuiFileDialog::Instance()->GetFilePathName();
  //     std::string file_path = ImGuiFileDialog::Instance()->GetCurrentPath();

  //     this->p_vtree->save_state(file_path_name);
  //     this->p_vtree->json_filename = file_path_name;
  //   }

  //   ImGuiFileDialog::Instance()->Close();
  // }

  return true;
}

void NodeEditor::render_links()
{
  for (auto &[link_id, link] : this->p_vtree->get_links_infos())
  {
    // change color of "dead" links_infos when a node upstream is frozen
    bool is_link_frozen =
        this->p_vtree->get_node_ref_by_id(link.node_id_from)->frozen_outputs;

    int dtype = this->p_vtree->get_node_ref_by_id(link.node_id_from)
                    ->get_port_ref_by_id(link.port_id_from)
                    ->dtype;

    ImVec4 color = ImColor(hesiod::vnode::dtype_colors.at(dtype).hovered);

    if (is_link_frozen)
      color = ImVec4(0.6f, 0.2f, 0.2f, 1.f);

    ax::NodeEditor::Link(ax::NodeEditor::LinkId(link_id),
                         ax::NodeEditor::PinId(link.port_hash_id_from),
                         ax::NodeEditor::PinId(link.port_hash_id_to),
                         color);
  }
}

void NodeEditor::render_view_node(std::string node_id)
{
  this->p_vtree->get_node_ref_by_id<hesiod::vnode::ViewNode>(node_id)
      ->render_node();
}

void NodeEditor::render_view_nodes()
{
  for (auto &[id, node] : this->p_vtree->get_nodes_map())
    this->p_vtree->get_node_ref_by_id<hesiod::vnode::ViewNode>(id)
        ->render_node();
}

} // namespace hesiod::gui
