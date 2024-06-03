/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

IntAttribute::IntAttribute(int value, int vmin, int vmax)
    : value(value), vmin(vmin), vmax(vmax)
{
}

int IntAttribute::get() { return value; }

} // namespace hesiod
