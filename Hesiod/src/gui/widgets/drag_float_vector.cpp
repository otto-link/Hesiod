/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include "highmap.hpp"

#include "hesiod/gui.hpp"

namespace hesiod::gui
{

bool drag_float_vector(std::vector<float> &vector,
                       bool                monotonic,
                       bool                variable_size,
                       float               vmin,
                       float               vmax,
                       bool                horizontal,
                       float               height)
{
  bool ret = false;
  ImGui::PushID((void *)&vector);
  ImGui::BeginGroup();

  float width = ImGui::GetContentRegionAvail().x / (float)vector.size() -
                ImGui::GetStyle().ItemSpacing.x;

  if (variable_size)
  {
    if (ImGui::Button("Size +1"))
    {
      vector.push_back(vmax);
      ret = true;
    }
    if (ImGui::Button("Size -1") && vector.size() > 1)
    {
      vector.pop_back();
      ret = true;
    }
  }

  if (ImGui::Button("Reset"))
  {
    for (auto &v : vector)
      v = vmax;
    ret = true;
  }

  if (horizontal)
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

  for (size_t k = 0; k < vector.size(); k++)
  {
    std::string stext = "##vslider" + std::to_string(k);

    if (horizontal)
    {
      ImGui::SliderFloat(stext.c_str(), &vector[k], vmin, vmax, "%.2f");
      if (ImGui::IsItemDeactivatedAfterEdit())
        ret = true;
    }
    else
    {
      ImGui::VSliderFloat(stext.c_str(),
                          ImVec2(width, height),
                          &vector[k],
                          vmin,
                          vmax,
                          "%.2f");
      if (ImGui::IsItemDeactivatedAfterEdit())
        ret = true;
      if (k < vector.size() - 1)
        ImGui::SameLine();
    }

    if (ImGui::IsItemActive())
    {
      if (monotonic)
      {
        // make sure the curve is monotonic
        if (k == 0)
          vector[k] = std::clamp(vector[k], 0.f, vector[k + 1]);
        else if (k == vector.size() - 1)
          vector[k] = std::clamp(vector[k], vector[k - 1], 1.f);
        else
          vector[k] = std::clamp(vector[k], vector[k - 1], vector[k + 1]);
      }
    }
  }

  if (horizontal)
    ImGui::PopItemWidth();

  ImGui::EndGroup();
  ImGui::PopID();

  return ret;
}

} // namespace hesiod::gui
