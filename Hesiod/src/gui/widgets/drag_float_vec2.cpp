/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include "highmap.hpp"

#include "hesiod/widgets.hpp"

namespace hesiod::gui
{

bool drag_float_vec2(hmap::Vec2<float> &kw, bool &link_xy)
{
  bool ret = false;
  ImGui::PushID((void *)&kw);
  ImGui::BeginGroup();

  ImGui::Checkbox("Link x and y", &link_xy);

  if (link_xy)
  {
    ImGui::DragFloat("kx, ky",
                     &kw.x,
                     0.1f,
                     0.001f,
                     64.f,
                     "%.1f",
                     ImGuiSliderFlags_None);

    if (ImGui::IsItemDeactivatedAfterEdit())
    {
      kw.y = kw.x;
      ret = true;
    }
  }
  else
  {
    ImGui::DragFloat2("kx, ky",
                      (float *)&kw,
                      0.1f,
                      0.001f,
                      64.f,
                      "%.1f",
                      ImGuiSliderFlags_None);

    if (ImGui::IsItemDeactivatedAfterEdit())
      ret = true;
  }

  ImGui::EndGroup();
  ImGui::PopID();

  return ret;
}

} // namespace hesiod::gui
