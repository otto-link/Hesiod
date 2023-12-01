/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SelectGradientNorm::SelectGradientNorm(std::string id) : Mask(id)
{
  this->node_type = "SelectGradientNorm";
  this->category = category_mapping.at(this->node_type);

  this->attr_ordered_key = {"inverse",
                            "smoothing",
                            "_ir_smoothing",
                            "saturate",
                            "_k_saturate",
                            "remap"};

  this->update_inner_bindings();
}

void SelectGradientNorm::compute_mask(hmap::HeightMap &h_out,
                                      hmap::HeightMap *p_input)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::transform(h_out,    // output
                  *p_input, // input
                  [](hmap::Array &array)
                  { return hmap::gradient_norm(array); });
}

} // namespace hesiod::cnode
