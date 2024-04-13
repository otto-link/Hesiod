/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <vector>

#include "macrologger.h"

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

MatrixAttribute::MatrixAttribute() {}

std::vector<std::vector<float>> MatrixAttribute::get() { return value; }

} // namespace hesiod
