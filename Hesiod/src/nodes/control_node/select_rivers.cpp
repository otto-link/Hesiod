/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SelectRivers::SelectRivers(std::string id) : ControlNode(id), Mask(id)
{
  this->node_type = "SelectRivers";
  this->category = category_mapping.at(this->node_type);

  this->attr["talus_ref"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 10.f);
  this->attr["clipping_ratio"] = NEW_ATTR_FLOAT(50.f, 0.1f, 1000.f);

  this->attr_ordered_key = {"talus_ref",
                            "clipping_ratio",
                            "inverse",
                            "smoothing",
                            "_ir_smoothing",
                            "saturate",
                            "_k_saturate",
                            "remap"};

  this->update_inner_bindings();
}

void SelectRivers::compute_mask(hmap::HeightMap &h_out,
                                hmap::HeightMap *p_input)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  {
    // hmap::transform(h_out,    // output
    //                 *p_input, // input
    //                 [this](hmap::Array &array) {
    //                   return hmap::select_rivers(array,
    //                                              this->talus_ref,
    //                                              this->clipping_ratio);
    //                 });
  }

  // --- work on a single array as a temporary solution
  hmap::Array z_array = p_input->to_array();
  z_array = hmap::select_rivers(z_array,
                                GET_ATTR_FLOAT("talus_ref"),
                                GET_ATTR_FLOAT("clipping_ratio"));
  h_out.from_array_interp(z_array);
}

} // namespace hesiod::cnode
