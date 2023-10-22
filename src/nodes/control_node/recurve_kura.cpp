/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

RecurveKura::RecurveKura(std::string id) : Filter(id)
{
  LOG_DEBUG("RecurveKura::RecurveKura()");
  this->node_type = "RecurveKura";
  this->category = category_mapping.at(this->node_type);
}

void RecurveKura::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  float hmin = h.min();
  float hmax = h.max();

  h.remap(0.f, 1.f);

  hmap::transform(h,
                  p_mask,
                  [this](hmap::Array &x, hmap::Array *p_mask)
                  { hmap::recurve_kura(x, a, b, p_mask); });

  h.remap(hmin, hmax);
}

} // namespace hesiod::cnode
