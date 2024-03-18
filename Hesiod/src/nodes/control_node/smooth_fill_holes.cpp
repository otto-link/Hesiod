/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SmoothFillHoles::SmoothFillHoles(std::string id) : ControlNode(id), Filter(id)
{
  LOG_DEBUG("SmoothFillHoles::SmoothFillHoles()");
  this->node_type = "SmoothFillHoles";
  this->category = category_mapping.at(this->node_type);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.001f, 0.2f, "%.3f");
}

void SmoothFillHoles::compute_filter(hmap::HeightMap &h,
                                     hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * h.shape.x));

  hmap::transform(h,
                  p_mask,
                  [this, &ir](hmap::Array &x, hmap::Array *p_mask)
                  { hmap::smooth_fill_holes(x, ir, p_mask); });

  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
