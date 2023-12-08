/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SelectPulse::SelectPulse(std::string id) : Mask(id)
{
  this->node_type = "SelectPulse";
  this->category = category_mapping.at(this->node_type);

  this->attr["value"] = NEW_ATTR_FLOAT(0.5f, -1.f, 2.f);
  this->attr["sigma"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);

  this->attr_ordered_key = {"value",
                            "sigma",
                            "inverse",
                            "smoothing",
                            "_ir_smoothing",
                            "saturate",
                            "_k_saturate",
                            "remap"};

  this->update_inner_bindings();
}

void SelectPulse::compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_input)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::transform(h_out,    // output
                  *p_input, // input
                  [this](hmap::Array &array)
                  {
                    return select_pulse(array,
                                        GET_ATTR_FLOAT("value"),
                                        GET_ATTR_FLOAT("sigma"));
                  });
}

} // namespace hesiod::cnode
