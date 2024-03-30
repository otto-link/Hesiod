/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/op.hpp"

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

void post_process_heightmap(hmap::HeightMap  &h,
                            bool              inverse,
                            bool              smoothing,
                            int               ir_smoothing,
                            bool              saturate,
                            hmap::Vec2<float> saturate_range,
                            float             saturate_k,
                            bool              remap,
                            hmap::Vec2<float> remap_range)
{
  if (inverse)
    h.inverse();

  if (smoothing)
  {
    hmap::transform(h,
                    [&ir_smoothing](hmap::Array &array)
                    { return hmap::smooth_cpulse(array, ir_smoothing); });
    h.smooth_overlap_buffers();
  }

  if (saturate)
  {
    float hmin = h.min();
    float hmax = h.max();

    // node parameters are assumed normalized and thus in [0, 1],
    // they need to be rescaled
    float smin_n = hmin + saturate_range.x * (hmax - hmin);
    float smax_n = hmax - (1.f - saturate_range.y) * (hmax - hmin);
    float k_n = saturate_k * (hmax - hmin);

    hmap::transform(h,
                    [&smin_n, &smax_n, &k_n](hmap::Array &array)
                    { hmap::clamp_smooth(array, smin_n, smax_n, k_n); });

    // keep original amplitude
    h.remap(hmin, hmax);
  }

  if (remap)
    h.remap(remap_range.x, remap_range.y);
}

} // namespace hesiod
