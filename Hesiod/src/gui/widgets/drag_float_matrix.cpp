/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include "hesiod/widgets.hpp"

namespace hesiod::gui
{

bool drag_float_matrix(std::vector<std::vector<float>> &matrix)
{
  bool ret = false;
  int  ni = (int)matrix.size();
  int  nj = ni > 0 ? (int)matrix[0].size() : 0;

  ImGui::PushID((void *)&matrix);
  ImGui::BeginGroup();

  ImGui::TextUnformatted("Value matrix");

  if (ImGui::InputInt("nx", &ni) || ImGui::InputInt("ny", &nj))
  {
    ni = ni < 1 ? 1 : ni;
    nj = nj < 1 ? 1 : nj;

    matrix.resize(ni);
    for (auto &row : matrix)
      row.resize(nj);
    ret = true;
  }

  ImGui::PushItemWidth(0.8f * ImGui::GetWindowWidth() / (float)ni);
  for (int j = nj - 1; j > -1; j--)
    for (int i = 0; i < ni; i++)
    {
      ImGui::PushID(i * nj + j);
      ret |=
          ImGui::SliderFloat("##dragfloat", &matrix[i][j], -1.f, 1.f, "%.2f");
      if (i < ni - 1)
        ImGui::SameLine();
      ImGui::PopID();
    }
  ImGui::PopItemWidth();

  if (ImGui::Button("Reset"))
  {
    ret = true;
    for (auto &row : matrix)
      for (auto &v : row)
        v = 0.f;
  }

  ImGui::EndGroup();
  ImGui::PopID();

  return ret;
}

} // namespace hesiod::gui
