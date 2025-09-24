/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <memory>

#include "highmap/array.hpp"

namespace hesiod
{

std::vector<uint8_t> generate_selector_image(hmap::Array &array);

}