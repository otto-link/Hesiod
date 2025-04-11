/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file cli.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

namespace hesiod::cli
{

void run_batch_mode(const std::string     &filename,
                    const hmap::Vec2<int> &shape,
                    const hmap::Vec2<int> &tiling,
                    float                  overlap);

}