/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

FloatAttribute::FloatAttribute(float value, float vmin, float vmax, std::string fmt)
    : value(value), vmin(vmin), vmax(vmax), fmt(fmt)
{
}

float FloatAttribute::get() { return value; }

} // namespace hesiod
