/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ValleyWidth::ValleyWidth(std::string id) : ControlNode(id), Mask(id)
{
  this->node_type = "ValleyWidth";
  this->category = category_mapping.at(this->node_type);

  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);

  this->attr_ordered_key = {"radius",
                            "inverse",
                            "smoothing",
                            "_ir_smoothing",
                            "saturate",
                            "_k_saturate",
                            "remap"};
}

void ValleyWidth::compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * h_out.shape.x));

  hmap::transform(h_out,
                  *p_h_in,
                  [&ir](hmap::Array &out, hmap::Array &in)
                  { out = hmap::valley_width(in, ir); });
}

} // namespace hesiod::cnode
