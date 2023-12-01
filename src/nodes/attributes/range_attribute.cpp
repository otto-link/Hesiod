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

RangeAttribute::RangeAttribute(hmap::Vec2<float> value) : value(value)
{
}

RangeAttribute::RangeAttribute(bool activate) : activate(activate)
{
}

hmap::Vec2<float> RangeAttribute::get()
{
  return value;
}

bool RangeAttribute::render_settings(std::string label)
{
  bool has_changed = false;

  if (ImGui::Checkbox(label.c_str(), &this->activate))
    has_changed = true;

  // if (this->activate)
  {
    ImGui::Indent();
    if (hesiod::gui::slider_vmin_vmax(this->value.x, this->value.y))
      has_changed = true;
    ImGui::Unindent();
  }

  return has_changed;
}

} // namespace hesiod
