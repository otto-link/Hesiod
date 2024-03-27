/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

VecFloatAttribute::VecFloatAttribute() {}

VecFloatAttribute::VecFloatAttribute(std::vector<float> value) : value(value) {}

std::vector<float> VecFloatAttribute::get() { return value; }

} // namespace hesiod
