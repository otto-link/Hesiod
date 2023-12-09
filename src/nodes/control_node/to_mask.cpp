/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ToMask::ToMask(std::string id) : ControlNode(id), Mask(id)
{
  this->node_type = "ToMask";
  this->category = category_mapping.at(this->node_type);
  this->attr_ordered_key = {"inverse",
                            "smoothing",
                            "_ir_smoothing",
                            "saturate",
                            "_k_saturate",
                            "remap"};
  this->update_inner_bindings();
}

void ToMask::compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_input)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  h_out = *p_input;
}

} // namespace hesiod::cnode
