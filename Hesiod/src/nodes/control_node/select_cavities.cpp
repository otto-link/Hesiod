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

  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.001f, 0.2f, "%.3f");
  this->attr["concave"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"radius",
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

  int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * h_out.shape.x));

  hmap::transform(
      h_out,    // output
      *p_input, // input
      [this, &ir](hmap::Array &array)
      { return hmap::select_cavities(array, ir, GET_ATTR_BOOL("concave")); });
}

} // namespace hesiod::cnode
