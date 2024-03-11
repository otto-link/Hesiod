/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

MeanLocal::MeanLocal(std::string id) : ControlNode(id), Filter(id)
{
  LOG_DEBUG("MeanLocal::MeanLocal()");
  this->node_type = "MeanLocal";
  this->category = category_mapping.at(this->node_type);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.02f, 0.f, 0.2f);
}

void MeanLocal::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * h.shape.x));

  // TODO add mask

  hmap::transform(h,
                  p_mask,
                  [this, &ir](hmap::Array &x, hmap::Array *p_mask)
                  { x = hmap::mean_local(x, ir); });
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
