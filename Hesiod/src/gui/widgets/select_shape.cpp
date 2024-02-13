/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include "hesiod/gui.hpp"

#define DEFAULT_SHAPE 256

namespace hesiod::gui
{

bool select_shape(std::string      label,
                  hmap::Vec2<int> &shape,
                  hmap::Vec2<int>  shape_max)
{
  bool has_changed = false;

  ImGui::PushID((void *)&shape);
  ImGui::BeginGroup();

  // redefine max resolution if the input shape is not consistent
  // (basically {0, 0} is the input field of the node is not defined)
  int sx = shape_max.x;
  int sy = shape_max.y;

  if (shape_max.x == 0)
    sx = DEFAULT_SHAPE;

  if (shape_max.y == 0)
    sy = DEFAULT_SHAPE;

  // create resolution list
  std::string str_default = std::to_string(shape.x) + "x" +
                            std::to_string(shape.y);

  if (ImGui::BeginCombo(label.c_str(), str_default.c_str()))
  {
    while (sx > 32 && sy > 32)
    {
      bool is_selected = (sx == shape.x && sy == shape.y);

      std::string str = std::to_string(sx) + "x" + std::to_string(sy);
      if (ImGui::Selectable(str.c_str(), is_selected))
      {
        shape = {sx, sy};
        has_changed = true;
      }

      if (is_selected)
        ImGui::SetItemDefaultFocus();

      sx /= 2;
      sy /= 2;
    }
    ImGui::EndCombo();
  }

  ImGui::EndGroup();
  ImGui::PopID();

  return has_changed;
}

} // namespace hesiod::gui
