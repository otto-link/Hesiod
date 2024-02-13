/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

void ControlNode::post_process_heightmap(hmap::HeightMap &h)
{
  if (this->attr.contains("inverse"))
    if (GET_ATTR_BOOL("inverse"))
      h.inverse();

  if (this->attr.contains("smoothing"))
    if (GET_ATTR_BOOL("smoothing"))
    {
      int ir_smoothing = GET_ATTR_INT("ir_smoothing");

      hmap::transform(h,
                      [&ir_smoothing](hmap::Array &array)
                      { return hmap::smooth_cpulse(array, ir_smoothing); });
      h.smooth_overlap_buffers();
    }

  if (this->attr.contains("saturate"))
    if (GET_ATTR_REF_RANGE("saturate")->is_activated())
    {
      hmap::Vec2<float> srange = GET_ATTR_RANGE("saturate");
      float             k = GET_ATTR_FLOAT("k_saturate");

      float hmin = h.min();
      float hmax = h.max();

      // node parameters are assumed normalized and thus in [0, 1],
      // they need to be rescaled
      float smin_n = hmin + srange.x * (hmax - hmin);
      float smax_n = hmax - (1.f - srange.y) * (hmax - hmin);
      float k_n = k * (hmax - hmin);

      hmap::transform(h,
                      [&smin_n, &smax_n, &k_n](hmap::Array &array)
                      { hmap::clamp_smooth(array, smin_n, smax_n, k_n); });

      // keep original amplitude
      h.remap(hmin, hmax);
    }

  if (this->attr.contains("remap"))
    if (GET_ATTR_REF_RANGE("remap")->is_activated())
    {
      hmap::Vec2<float> vrange = GET_ATTR_RANGE("remap");
      h.remap(vrange.x, vrange.y);
    }
}

} // namespace hesiod::cnode
