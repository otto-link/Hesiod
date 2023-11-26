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

WaveNbAttribute::WaveNbAttribute(hmap::Vec2<float> value) : value(value)
{
}

hmap::Vec2<float> WaveNbAttribute::get()
{
  return value;
}

bool WaveNbAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  has_changed |= hesiod::gui::drag_float_vec2(this->value, this->link_xy);
  return has_changed;
}

} // namespace hesiod
