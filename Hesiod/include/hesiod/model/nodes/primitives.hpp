/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file primitives.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

struct Noise : public BaseNode
{
  Noise(std::shared_ptr<ModelConfig> config);

  void compute() override;
};

struct NoiseFbm : public BaseNode
{
  NoiseFbm(std::shared_ptr<ModelConfig> config);

  void compute() override;
};

} // namespace hesiod