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

IntAttribute::IntAttribute(int value, int vmin, int vmax)
    : value(value), vmin(vmin), vmax(vmax)
{
}

int IntAttribute::get()
{
  return value;
}

bool IntAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  ImGui::SliderInt(label.c_str(), &this->value, this->vmin, this->vmax);
  has_changed |= ImGui::IsItemDeactivatedAfterEdit();
  return has_changed;
}

} // namespace hesiod
