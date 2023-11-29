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

ShapeAttribute::ShapeAttribute()
{
}

hmap::Vec2<int> ShapeAttribute::get()
{
  return value;
}

void ShapeAttribute::set_value_max(hmap::Vec2<int> new_value_max)
{
  this->value_max = new_value_max;
}

bool ShapeAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  has_changed |= hesiod::gui::select_shape(label.c_str(),
                                           this->value,
                                           this->value_max);
  return has_changed;
}

} // namespace hesiod
