/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

MinimumLocal::MinimumLocal(std::string id) : ControlNode(id), Unary(id)
{
  LOG_DEBUG("MinimumLocal::MinimumLocal()");
  this->node_type = "MinimumLocal";
  this->category = category_mapping.at(this->node_type);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.02f, 0.f, 0.5f);
}

void MinimumLocal::compute_in_out(hmap::HeightMap &h_out,
                                  hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * h_out.shape.x));

  // make a copy of the input and applied range remapping
  hmap::transform(h_out,
                  *p_h_in,
                  [this, &ir](hmap::Array &x, hmap::Array &y)
                  { x = hmap::minimum_local(y, ir); });
  h_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
