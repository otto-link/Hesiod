/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"
#include "hesiod/windows.hpp"

namespace hesiod::gui
{

ShortcutGroupId NodeList::get_element_shortcut_group_id()
{
  return "GroupNodeList" + this->get_unique_id();
}

bool NodeList::initialize()
{
  LOG_DEBUG("initializing NodeList window for ViewTree [%s]",
            p_vtree->id.c_str());

  this->title = "Node listing " + p_vtree->id;
  this->flags = ImGuiWindowFlags_MenuBar;

  return true;
}

bool NodeList::render_content()
{
  ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter |
                          ImGuiTableFlags_BordersV;
  const int ncol = 7;

  if (ImGui::BeginTable(("table##" + this->p_vtree->id).c_str(), ncol, flags))
  {
    ImGui::TableSetupColumn("Hash Id");
    ImGui::TableSetupColumn("Node Id");
    ImGui::TableSetupColumn("Port Id");
    ImGui::TableSetupColumn("Port Direction");
    ImGui::TableSetupColumn("LinkInfos");
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

    for (auto &[id, vnode] : this->p_vtree->get_nodes_map())
    {
      ImGui::TableNextRow();

      ImGui::TableNextColumn();
      ImGui::TextColored(ImColor(IM_COL32(255, 255, 255, 100)),
                         "%d",
                         vnode.get()->hash_id);
      ImGui::TableNextColumn();
      ImGui::SameLine();
      ImGui::TextUnformatted(id.c_str());

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

  return true;
}

} // namespace hesiod::gui
