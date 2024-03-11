/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Plateau::Plateau(std::string id) : ControlNode(id), Filter(id)
{
  this->node_type = "Plateau";
  this->category = category_mapping.at(this->node_type);

  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.01f, 0.2f);
  this->attr["factor"] = NEW_ATTR_FLOAT(4.f, 0.01f, 10.f);

  this->attr_ordered_key = {"radius", "factor"};
}

void Plateau::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * h.shape.x));

  hmap::transform(h,
                  p_mask,
                  [this, &ir](hmap::Array &x, hmap::Array *p_mask)
                  { hmap::plateau(x, p_mask, ir, GET_ATTR_FLOAT("factor")); });
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
