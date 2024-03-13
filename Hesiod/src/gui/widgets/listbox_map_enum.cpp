/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include "hesiod/widgets.hpp"

namespace hesiod::gui
{

bool listbox_map_enum(std::map<std::string, int> &map,
                      int                        &selected,
                      float                       width)
{
  bool has_changed = false;

  ImGui::PushID((void *)&map);

  int n_items = std::min(8, (int)map.size());

  float height = ImGui::GetStyle().ItemSpacing.y +
                 (float)n_items * ImGui::GetTextLineHeightWithSpacing();

  if (ImGui::BeginListBox("##method", ImVec2(width, height)))
  {
    for (auto &[cname, k] : map)
    {
      bool is_selected = false;
      if (selected == k)
        is_selected = true;

      if (ImGui::Selectable(cname.c_str(), is_selected))
      {
        selected = k;
        has_changed = true;
      }
    }
    ImGui::EndListBox();
  }

  ImGui::PopID();

  return has_changed;
}

} // namespace hesiod::gui
