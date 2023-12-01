/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Recurve::Recurve(std::string id) : Filter(id)
{
  LOG_DEBUG("Recurve::Recurve()");
  this->node_type = "Recurve";
  this->category = category_mapping.at(this->node_type);

  this->attr["curve"] = NEW_ATTR_VECFLOAT(std::vector<float>({0.f, 0.5f, 1.f}));
}

void Recurve::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  float hmin = h.min();
  float hmax = h.max();

  std::vector<float> curve = GET_ATTR_VECFLOAT("curve");
  std::vector<float> t = hmap::linspace(hmin, hmax, (int)curve.size());

  // rescale curve (given in [0, 1] in the GUI)
  std::vector<float> scaled_curve(curve.size());
  for (size_t k = 0; k < curve.size(); k++)
    scaled_curve[k] = curve[k] * (hmax - hmin) + hmin;

  hmap::transform(h,
                  p_mask,
                  [&t, &scaled_curve, this](hmap::Array &x, hmap::Array *p_mask)
                  { hmap::recurve(x, t, scaled_curve, p_mask); });
}

} // namespace hesiod::cnode
