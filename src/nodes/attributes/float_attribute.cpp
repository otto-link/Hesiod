/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "macrologger.h"
#include <imgui.h>

#include "hesiod/attribute.hpp"
#include "hesiod/gui.hpp"

namespace hesiod
{

FloatAttribute::FloatAttribute(float value, float vmin, float vmax)
    : value(value), vmin(vmin), vmax(vmax)
{
}

float FloatAttribute::get()
{
  return value;
}

bool FloatAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  ImGui::SliderFloat(label.c_str(),
                     &this->value,
                     this->vmin,
                     this->vmax,
                     "%.2f");
  has_changed |= ImGui::IsItemDeactivatedAfterEdit();
  return has_changed;
}

} // namespace hesiod
