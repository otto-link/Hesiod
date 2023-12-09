/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "macrologger.h"
#include <imgui.h>

#include "hesiod/attribute.hpp"

namespace hesiod
{

std::vector<float> ColorAttribute::get()
{
  return value;
}

bool ColorAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  ImGui::ColorEdit3(label.c_str(), this->value.data());
  has_changed |= ImGui::IsItemDeactivatedAfterEdit();
  return has_changed;
}

} // namespace hesiod
