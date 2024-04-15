/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

RangeAttribute::RangeAttribute(hmap::Vec2<float> value, std::string fmt)
    : value(value), fmt(fmt)
{
}

RangeAttribute::RangeAttribute(std::string fmt) : fmt(fmt) {}

hmap::Vec2<float> RangeAttribute::get() { return value; }

} // namespace hesiod