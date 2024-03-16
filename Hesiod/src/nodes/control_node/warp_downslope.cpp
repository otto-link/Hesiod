/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

WarpDownslope::WarpDownslope(std::string id) : ControlNode(id), Filter(id)
{
  this->node_type = "WarpDownslope";
  this->category = category_mapping.at(this->node_type);

  this->attr["amount"] = NEW_ATTR_FLOAT(0.01f, 0.f, 0.2f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.f, 0.f, 1.f);
  this->attr["inverse"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"amount", "radius", "inverse"};
}

void WarpDownslope::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  int ir = (int)(GET_ATTR_FLOAT("radius") * h.shape.x);

  hmap::transform(h,
                  p_mask,
                  [this, &ir](hmap::Array &x, hmap::Array *p_mask)
                  {
                    hmap::warp_downslope(x,
                                         p_mask,
                                         GET_ATTR_FLOAT("amount"),
                                         ir,
                                         GET_ATTR_BOOL("inverse"));
                  });
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
