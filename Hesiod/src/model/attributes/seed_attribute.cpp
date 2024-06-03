/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

SeedAttribute::SeedAttribute(uint value) : value(value) {}

uint SeedAttribute::get() { return value; }

} // namespace hesiod
