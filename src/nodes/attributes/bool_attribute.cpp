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

BoolAttribute::BoolAttribute(bool value) : value(value)
{
}

bool BoolAttribute::get()
{
  return value;
}

bool BoolAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  has_changed |= ImGui::Checkbox(label.c_str(), &this->value);
  return has_changed;
}

} // namespace hesiod
