/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#define _USE_MATH_DEFINES
#include <cmath>

#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SteepenConvective::SteepenConvective(std::string id)
    : ControlNode(id), Filter(id)
{
  LOG_DEBUG("SteepenConvective::SteepenConvective()");
  this->node_type = "SteepenConvective";
  this->category = category_mapping.at(this->node_type);

  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);
  this->attr["iterations"] = NEW_ATTR_INT(1, 1, 100);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.f, 0.f, 0.5f);
  this->attr["dt"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 2.f);

  this->attr_ordered_key = {"angle", "iterations", "radius", "dt"};
}

void SteepenConvective::compute_filter(hmap::HeightMap &h,
                                       hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * h.shape.x));

  float hmin = h.min();
  float hmax = h.max();
  h.remap(0.f, 1.f, hmin, hmax);
  hmap::transform(h,
                  p_mask,
                  [this, &ir](hmap::Array &x, hmap::Array *p_mask)
                  {
                    hmap::steepen_convective(x,
                                             GET_ATTR_FLOAT("angle"),
                                             p_mask,
                                             GET_ATTR_INT("iterations"),
                                             ir,
                                             GET_ATTR_FLOAT("dt"));
                  });
  h.smooth_overlap_buffers();
  h.remap(hmin, hmax, 0.f, 1.f);
}

} // namespace hesiod::cnode
