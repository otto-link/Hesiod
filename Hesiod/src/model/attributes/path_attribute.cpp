/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <vector>

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

hmap::Path PathAttribute::get() { return value; }

} // namespace hesiod
