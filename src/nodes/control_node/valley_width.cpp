/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ValleyWidth::ValleyWidth(std::string id) : Mask(id)
{
  this->node_type = "ValleyWidth";
  this->category = category_mapping.at(this->node_type);

  this->attr["ir"] = NEW_ATTR_INT(4, 1, 128);

  this->attr_ordered_key = {"ir",
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

  hmap::transform(h_out,
                  *p_h_in,
                  [this](hmap::Array &out, hmap::Array &in)
                  { out = hmap::valley_width(in, GET_ATTR_INT("ir")); });
}

} // namespace hesiod::cnode
