/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

StringAttribute::StringAttribute(std::string value) : value(value) {}

std::string StringAttribute::get() { return value; }

} // namespace hesiod
