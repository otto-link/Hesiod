/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

VecIntAttribute::VecIntAttribute() {}

VecIntAttribute::VecIntAttribute(std::vector<int> value) : value(value) {}

std::vector<int> VecIntAttribute::get() { return value; }

} // namespace hesiod
