/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/blending.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/range.hpp"

#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

void blend_heightmaps(hmap::Heightmap &h_out,
                      hmap::Heightmap &h1,
                      hmap::Heightmap &h2,
                      BlendingMethod   method,
                      float            k,
                      int              ir)
{
  std::function<void(hmap::Array &, hmap::Array &, hmap::Array &)> lambda;

  switch (method)
  {
  case BlendingMethod::ADD:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 + a2; };
    break;

  case BlendingMethod::EXCLUSION_BLEND:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::blend_exclusion(a1, a2); };
    break;

  case BlendingMethod::GRADIENTS:
    lambda = [&ir](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::blend_gradients(a1, a2, ir); };
    break;

  case BlendingMethod::MAXIMUM:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::maximum(a1, a2); };
    break;

  case BlendingMethod::MAXIMUM_SMOOTH:
    lambda = [&k](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::maximum_smooth(a1, a2, k); };
    break;

  case BlendingMethod::MINIMUM:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::minimum(a1, a2); };
    break;

  case BlendingMethod::MINIMUM_SMOOTH:
    lambda = [&k](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::minimum_smooth(a1, a2, k); };
    break;

  case BlendingMethod::MULTIPLY:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 * a2; };
    break;

  case BlendingMethod::MULTIPLY_ADD:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 + a1 * a2; };
    break;

  case BlendingMethod::NEGATE:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::blend_negate(a1, a2); };
    break;

  case BlendingMethod::OVERLAY:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::blend_overlay(a1, a2); };
    break;

  case BlendingMethod::SOFT:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::blend_soft(a1, a2); };
    break;

  case BlendingMethod::REPLACE:
    lambda = [](hmap::Array &m, hmap::Array & /* a1 */, hmap::Array &a2) { m = a2; };
    break;

  case BlendingMethod::SUBSTRACT:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2) { m = a1 - a2; };
    break;
  }

  hmap::transform(h_out, h1, h2, lambda);

  if (method == BlendingMethod::GRADIENTS)
    h_out.smooth_overlap_buffers();
}

} // namespace hesiod
