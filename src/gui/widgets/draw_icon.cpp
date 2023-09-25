/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include "highmap.hpp"

#include "hesiod/gui.hpp"

namespace hesiod::gui
{

void draw_icon(int icon_type, ImVec2 size, uint32_t color, bool filled)
{
  // if (ImGui::IsRectVisible(size))
  {
    ImDrawList *drawList = ImGui::GetWindowDrawList();

    ImVec2 pos = ImGui::GetCursorScreenPos();
    // "center" the icon vertical alignement
    pos.y += (ImGui::GetTextLineHeightWithSpacing() -
              ImGui::CalcTextSize("A").y);

    if (icon_type == hesiod::gui::square)
    {
      if (filled)
        drawList->AddRectFilled(pos,
                                {pos.x + size.x, pos.y + size.y},
                                color,
                                2.f);
      drawList
          ->AddRect(pos, {pos.x + size.x, pos.y + size.y}, color, 2.f, 0, 2.f);
    }
  }
  ImGui::Dummy({size.x, ImGui::GetTextLineHeightWithSpacing()});
}

} // namespace hesiod::gui
