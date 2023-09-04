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

// HELPER Comparator function to sort pairs according to second value
bool cmp_inf(std::pair<std::string, std::string> &a,
             std::pair<std::string, std::string> &b)
{
  return (a.second < b.second) |
         ((a.second == b.second) & (a.first < b.second));
}

bool cmp_sup(std::pair<std::string, std::string> &a,
             std::pair<std::string, std::string> &b)
{
  return (a.second > b.second) |
         ((a.second == b.second) & (a.first > b.second));
}

// HELPER Function to sort the map according to value in a (key-value)
// pairs
std::vector<std::string> sort(const std::map<std::string, std::string> &amap,
                              bool                                      reverse)
{
  std::vector<std::string> keys_sorted; // output
  keys_sorted.reserve(amap.size());
  std::vector<std::pair<std::string, std::string>> pairs;

  for (auto &it : amap)
    pairs.push_back(it);

  if (reverse)
    std::sort(pairs.begin(), pairs.end(), cmp_sup);
  else
    std::sort(pairs.begin(), pairs.end(), cmp_inf);

  for (auto &it : pairs)
    keys_sorted.push_back(it.first);
  return keys_sorted;
}

std::string ViewTree::render_new_node_treeview(const ImVec2 node_position)
{
  std::string new_node_id = "";
  const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

  ImGuiTableFlags flags = ImGuiTableFlags_Reorderable |
                          ImGuiTableFlags_Sortable |
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

    // sorting
    if (ImGuiTableSortSpecs *sorts_specs = ImGui::TableGetSortSpecs())
      if (sorts_specs->SpecsDirty)
      {
        // sort direction
        ImGuiSortDirection sort_dir = sorts_specs->Specs->SortDirection;
        bool               reverse = false;
        if (sort_dir == ImGuiSortDirection_Descending)
          reverse = true;

        // sorting
        if (sorts_specs->Specs->ColumnIndex == 1)
          this->key_sort = sort(hesiod::cnode::category_mapping, reverse);
        else
        {
          this->key_sort.clear();
          for (auto &[node_type, node_category] :
               hesiod::cnode::category_mapping)
            this->key_sort.push_back(node_type);
        }

        sorts_specs->SpecsDirty = false;
      }

    ImGuiListClipper clipper;
    clipper.Begin((int)hesiod::cnode::category_mapping.size());

    while (clipper.Step())
    {
      for (int k = clipper.DisplayStart; k < clipper.DisplayEnd; k++)
      {
        std::string node_type = this->key_sort[k];
        std::string node_category = hesiod::cnode::category_mapping.at(
            this->key_sort[k]);

        ImGui::PushID(k);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        if (ImGui::Selectable(node_type.c_str(),
                              false,
                              ImGuiSelectableFlags_SpanAllColumns))
        {
          LOG_DEBUG("selected node type: %s", node_type.c_str());
          new_node_id = this->add_view_node(node_type);

          // set node position: the node needs to be rendered first
          this->render_view_node(new_node_id);
          ax::NodeEditor::SetNodePosition(
              this->get_node_ref_by_id(new_node_id)->hash_id,
              ax::NodeEditor::ScreenToCanvas(node_position));
        }

        ImGui::TableNextColumn();
        std::string main_category = node_category.substr(
            0,
            node_category.find("/"));
        ImGui::TextColored(ImColor(category_colors.at(main_category).selected),
                           "%s",
                           node_category.c_str());
        ImGui::PopID();
      }
    }
    ImGui::EndTable();
  }

  return new_node_id;
}

} // namespace hesiod::vnode
