/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SelectInterval::SelectInterval(std::string id) : Mask(id)
{
  this->node_type = "SelectInterval";
  this->category = category_mapping.at(this->node_type);

  this->attr["value_low"] = NEW_ATTR_FLOAT(0.f, -1.f, 2.f);
  this->attr["value_high"] = NEW_ATTR_FLOAT(0.5f, -1.f, 2.f);

  this->attr_ordered_key = {"value_low",
                            "value_high",
                            "inverse",
                            "smoothing",
                            "_ir_smoothing",
                            "saturate",
                            "_k_saturate",
                            "remap"};

  this->update_inner_bindings();
}

void SelectInterval::compute_mask(hmap::HeightMap &h_out,
                                  hmap::HeightMap *p_input)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::transform(h_out,    // output
                  *p_input, // input
                  [this](hmap::Array &array)
                  {
                    return hmap::select_interval(array,
                                                 GET_ATTR_FLOAT("value_low"),
                                                 GET_ATTR_FLOAT("value_high"));
                  });
}

} // namespace hesiod::cnode
