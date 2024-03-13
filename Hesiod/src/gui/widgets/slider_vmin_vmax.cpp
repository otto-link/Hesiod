/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include "hesiod/widgets.hpp"

namespace hesiod::gui
{

bool slider_vmin_vmax(float &vmin, float &vmax)
{
  bool ret = false;

  ImGui::PushID((void *)&vmin);
  ImGui::BeginGroup();

  // ImGui::TextUnformatted("Range selection");

  ImGui::DragFloatRange2("range",
                         &vmin,
                         &vmax,
                         0.01f,
                         -FLT_MAX,
                         FLT_MAX,
                         "vmin: %.2f",
                         "vmax: %.2f",
                         ImGuiSliderFlags_AlwaysClamp);
  if (ImGui::IsItemDeactivatedAfterEdit())
    ret = true;

  float vmin_before_change = vmin;
  if (ImGui::DragFloat("offset", &vmin, 0.01f, -FLT_MAX, FLT_MAX, "%.2f"))
  {
    vmax += vmin - vmin_before_change;
    ret = true;
  }

  if (ImGui::Button("reset"))
  {
    vmin = 0.f;
    vmax = 1.f;
    ret = true;
  }

  ImGui::SameLine();
  if (ImGui::Button("center"))
  {
    float span = vmax - vmin;
    vmin = -0.5f * span;
    vmax = 0.5f * span;
    ret = true;
  }

  ImGui::EndGroup();
  ImGui::PopID();

  return ret;
}

} // namespace hesiod::gui
