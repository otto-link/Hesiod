/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include "highmap/primitives.hpp"

namespace hesiod
{

/**
 * @brief Plain text / enumerate mapping for the HighMap noise types.
 */
static std::map<std::string, int> noise_type_map = {
    {"Perlin", hmap::NoiseType::n_perlin},
    {"Perlin (billow)", hmap::NoiseType::n_perlin_billow},
    {"Perlin (half)", hmap::NoiseType::n_perlin_half},
    {"OpenSimplex2", hmap::NoiseType::n_simplex2},
    {"OpenSimplex2S", hmap::NoiseType::n_simplex2s},
    {"Value", hmap::NoiseType::n_value},
    {"Value (cubic)", hmap::NoiseType::n_value_cubic},
    {"Value (delaunay)", hmap::NoiseType::n_value_delaunay},
    {"Value (linear)", hmap::NoiseType::n_value_linear},
    {"Value (thinplate)", hmap::NoiseType::n_value_thinplate},
    {"Worley", hmap::NoiseType::n_worley},
    {"Worley (doube)", hmap::NoiseType::n_worley_double},
    {"Worley (value)", hmap::NoiseType::n_worley_value}};

} // namespace hesiod