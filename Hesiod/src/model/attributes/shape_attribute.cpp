/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

ShapeAttribute::ShapeAttribute() {}

hmap::Vec2<int> ShapeAttribute::get() { return value; }

void ShapeAttribute::set_value_max(hmap::Vec2<int> new_value_max)
{
  this->value_max = new_value_max;
}

} // namespace hesiod
