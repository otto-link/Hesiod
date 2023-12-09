/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Laplace::Laplace(std::string id) : ControlNode(id), Filter(id)
{
  this->node_type = "Laplace";
  this->category = category_mapping.at(this->node_type);
  this->attr["sigma"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);
  this->attr["iterations"] = NEW_ATTR_INT(3, 1, 10);
}

void Laplace::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());
  hmap::transform(h,
                  p_mask,
                  [this](hmap::Array &x, hmap::Array *p_mask)
                  {
                    hmap::laplace(x,
                                  p_mask,
                                  GET_ATTR_FLOAT("sigma"),
                                  GET_ATTR_INT("iterations"));
                  });
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
