/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SelectCavities::SelectCavities(std::string id) : ControlNode(id), Mask(id)
{
  this->node_type = "SelectCavities";
  this->category = category_mapping.at(this->node_type);

  this->attr["ir"] = NEW_ATTR_INT(32, 1, 256);
  this->attr["concave"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"ir",
                            "concave",
                            "inverse",
                            "smoothing",
                            "_ir_smoothing",
                            "saturate",
                            "_k_saturate",
                            "remap"};

  this->update_inner_bindings();
}

void SelectCavities::compute_mask(hmap::HeightMap &h_out,
                                  hmap::HeightMap *p_input)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::transform(h_out,    // output
                  *p_input, // input
                  [this](hmap::Array &array)
                  {
                    return hmap::select_cavities(array,
                                                 GET_ATTR_INT("ir"),
                                                 GET_ATTR_BOOL("concave"));
                  });
}

} // namespace hesiod::cnode
