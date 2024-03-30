/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file utils.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "highmap/heightmap.hpp"

namespace hesiod
{

bool convert_qjsonvalue_to_bool(QJsonValue v, bool &out);

bool convert_qjsonvalue_to_float(QJsonValue v, float &out);

bool convert_qjsonvalue_to_int(QJsonValue v, int &out);

bool convert_qjsonvalue_to_uint(QJsonValue v, uint &out);

bool convert_qjsonvalue_to_string(QJsonValue v, std::string &out);

/**
 * @brief
 * @param h
 * @param inverse
 * @param smoothing
 * @param ir_smoothing
 * @param saturate
 * @param saturate_range
 * @param saturate_k
 * @param remap
 * @param remap_range
 */
void post_process_heightmap(hmap::HeightMap  &h,
                            bool              inverse,
                            bool              smoothing,
                            int               ir_smoothing,
                            bool              saturate,
                            hmap::Vec2<float> saturate_range,
                            float             saturate_k,
                            bool              remap,
                            hmap::Vec2<float> remap_range);

} // namespace hesiod