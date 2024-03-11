/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Rugosity::Rugosity(std::string id) : ControlNode(id), Mask(id)
{
  this->node_type = "Rugosity";
  this->category = category_mapping.at(this->node_type);

  this->attr["radius"] = NEW_ATTR_FLOAT(0.1f, 0.f, 0.5f);
  this->attr["clamp_max"] = NEW_ATTR_BOOL(false);
  this->attr["vc_max"] = NEW_ATTR_FLOAT(1.f, 0.f, 10.f);

  this->attr_ordered_key = {"radius",
                            "clamp_max",
                            "_vc_max",
                            "inverse",
                            "smoothing",
                            "_ir_smoothing",
                            "saturate",
                            "_k_saturate",
                            "remap"};
}

void Rugosity::compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * h_out.shape.x));

  // make a copy of the input and applied range remapping
  hmap::transform(h_out,
                  *p_h_in,
                  [this, &ir](hmap::Array &out, hmap::Array &in)
                  { out = hmap::rugosity(in, ir); });

  if (GET_ATTR_BOOL("clamp_max"))
    hmap::transform(h_out,
                    [this](hmap::Array &x)
                    { hmap::clamp_max(x, GET_ATTR_FLOAT("vc_max")); });
}

} // namespace hesiod::cnode
