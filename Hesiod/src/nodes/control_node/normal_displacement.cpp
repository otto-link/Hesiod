/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

NormalDisplacement::NormalDisplacement(std::string id)
    : ControlNode(id), Filter(id)
{
  this->node_type = "NormalDisplacement";
  this->category = category_mapping.at(this->node_type);

  this->attr["radius"] = NEW_ATTR_FLOAT(0.f, 0.f, 1.f);
  this->attr["amount"] = NEW_ATTR_FLOAT(5.f, 0.f, 10.f);
  this->attr["reverse"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"radius", "amount", "reverse"};
}

void NormalDisplacement::compute_filter(hmap::HeightMap &h,
                                        hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  int ir = std::max(0, (int)(GET_ATTR_FLOAT("radius") * h.shape.x));

  hmap::transform(h,
                  p_mask,
                  [this, &ir](hmap::Array &x, hmap::Array *p_mask)
                  {
                    hmap::normal_displacement(x,
                                              p_mask,
                                              GET_ATTR_FLOAT("amount"),
                                              ir,
                                              GET_ATTR_BOOL("reverse"));
                  });
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
