/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

GammaCorrection::GammaCorrection(std::string id) : ControlNode(id), Filter(id)
{
  LOG_DEBUG("GammaCorrection::GammaCorrection()");
  this->node_type = "GammaCorrection";
  this->category = category_mapping.at(this->node_type);
  this->attr["gamma"] = NEW_ATTR_FLOAT(1.f, 0.01f, 10.f);
}

void GammaCorrection::compute_filter(hmap::HeightMap &h,
                                     hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  float hmin = h.min();
  float hmax = h.max();
  h.remap(0.f, 1.f, hmin, hmax);
  hmap::transform(h,
                  p_mask,
                  [this](hmap::Array &x, hmap::Array *p_mask) {
                    hmap::gamma_correction(x, GET_ATTR_FLOAT("gamma"), p_mask);
                  });
  h.remap(hmin, hmax, 0.f, 1.f);
}

} // namespace hesiod::cnode
