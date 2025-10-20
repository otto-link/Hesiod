/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
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

#include "hesiod/model/model_config.hpp"

namespace hesiod::cli
{

int parse_args(args::ArgumentParser &parser, int argc, char *argv[]);

void run_batch_mode(const std::string     &filename,
                    const hmap::Vec2<int> &shape,
                    const hmap::Vec2<int> &tiling,
                    float                  overlap,
                    const ModelConfig     *p_input_model_config = nullptr);
void run_node_inventory();
void run_snapshot_generation();

} // namespace hesiod::cli