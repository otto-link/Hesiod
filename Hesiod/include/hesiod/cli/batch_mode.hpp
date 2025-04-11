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
#include <fstream>

// in this order, required by args.hxx
#include "highmap/algebra.hpp"
static std::istream &operator>>(std::istream &is, hmap::Vec2<int> &vec2)
{
  is >> vec2.x;
  is.get();
  is >> vec2.y;
  return is;
}
#include <args.hxx>

namespace hesiod::cli
{

int parse_args(args::ArgumentParser &parser, int argc, char *argv[]);

void run_batch_mode(const std::string     &filename,
                    const hmap::Vec2<int> &shape,
                    const hmap::Vec2<int> &tiling,
                    float                  overlap);
void run_node_inventory();
void run_snapshot_generation();

} // namespace hesiod::cli