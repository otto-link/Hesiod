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

void ViewTree::render_node_list()
{
  ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter |
                          ImGuiTableFlags_BordersV;
  const int ncol = 7;

  // if (ImGui::Button("focus"))
  // {
  //   ImVec2 size = ImGui::GetContentRegionAvail();
  //   LOG_DEBUG("%f %f", size.x, size.y);

  //   ImVec2 panning = ImNodes::EditorContextGetPanning();
  //   LOG_DEBUG("%f %f", panning.x, panning.y);

  //   std::string nid = this->get_nodes_map().begin()->first;
  //   ImNodes::EditorContextMoveToNode(this->get_node_ref_by_id(nid)->hash_id);
  //   // TODO get canvas widget with ImGui and centre
  // }

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
}

} // namespace hesiod::vnode
