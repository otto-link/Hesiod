/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <chrono>

namespace hesiod
{
struct Timer
{
  std::chrono::high_resolution_clock::time_point t0;

  Timer()
  {
    this->t0 = std::chrono::high_resolution_clock::now();
  }

  void reset()
  {
    this->t0 = std::chrono::high_resolution_clock::now();
  }

  float stop()
  {
    std::chrono::high_resolution_clock::time_point t1 =
        std::chrono::high_resolution_clock::now();
    return (float)std::chrono::duration_cast<std::chrono::nanoseconds>(t1 -
                                                                       this->t0)
               .count() *
           1e-6f;
  }
};
} // namespace hesiod
