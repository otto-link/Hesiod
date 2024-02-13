/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SelectElevationSlope::SelectElevationSlope(std::string id)
    : ControlNode(id), Mask(id)
{
  this->node_type = "SelectElevationSlope";
  this->category = category_mapping.at(this->node_type);

  this->attr["vmax"] = NEW_ATTR_FLOAT(0.5f, -1.f, 2.f);
  this->attr["gradient_scaling"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 1.f);

  this->attr_ordered_key = {"vmax",
                            "gradient_scaling",
                            "inverse",
                            "smoothing",
                            "_ir_smoothing",
                            "saturate",
                            "_k_saturate",
                            "remap"};

  this->update_inner_bindings();
}

void SelectElevationSlope::compute_mask(hmap::HeightMap &h_out,
                                        hmap::HeightMap *p_input)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  float scaling = GET_ATTR_FLOAT("gradient_scaling") * (float)h_out.shape.x;

  hmap::transform(h_out,    // output
                  *p_input, // input
                  [this, &scaling](hmap::Array &array)
                  {
                    return hmap::select_elevation_slope(array,
                                                        GET_ATTR_FLOAT("vmax"),
                                                        scaling);
                  });
}

} // namespace hesiod::cnode
