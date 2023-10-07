/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#define _USE_MATH_DEFINES
#include <cmath>

#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SteepenConvective::SteepenConvective(std::string id) : Filter(id)
{
  LOG_DEBUG("SteepenConvective::SteepenConvective()");
  this->node_type = "SteepenConvective";
  this->category = category_mapping.at(this->node_type);
}

void SteepenConvective::compute_filter(hmap::HeightMap &h,
                                       hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  float hmin = h.min();
  float hmax = h.max();
  h.remap(0.f, 1.f, hmin, hmax);
  hmap::transform(h,
                  p_mask,
                  [this](hmap::Array &x, hmap::Array *p_mask)
                  {
                    hmap::steepen_convective(x,
                                             this->angle / M_PI * 180.f,
                                             p_mask,
                                             this->iterations,
                                             this->ir,
                                             this->dt);
                  });
  h.smooth_overlap_buffers();
  h.remap(hmin, hmax, 0.f, 1.f);
}

} // namespace hesiod::cnode
