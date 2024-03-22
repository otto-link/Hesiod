/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <vector>

#include "macrologger.h"

#include "hesiod/widgets.hpp"

namespace hesiod::gui
{

void render_colorbar(std::vector<std::vector<float>> colormap_data, ImVec2 size)
{
  if (ImGui::IsRectVisible(size))
  {
    ImDrawList *drawList = ImGui::GetWindowDrawList();

    ImVec2 pos = ImGui::GetCursorScreenPos();
    float  dx = size.x / (float)(colormap_data.size() - 1);

    for (size_t k = 0; k < colormap_data.size() - 1; k++)
    {
      ImU32 col_a = ImColor(colormap_data[k][0],
                            colormap_data[k][1],
                            colormap_data[k][2]);
      ImU32 col_b = ImColor(colormap_data[k + 1][0],
                            colormap_data[k + 1][1],
                            colormap_data[k + 1][2]);

      drawList->AddRectFilledMultiColor(pos,
                                        {pos.x + dx, pos.y + size.y},
                                        col_a,
                                        col_b,
                                        col_b,
                                        col_a);
      pos.x += dx;
    }
  }
  ImGui::Dummy(size);
}

} // namespace hesiod::gui
