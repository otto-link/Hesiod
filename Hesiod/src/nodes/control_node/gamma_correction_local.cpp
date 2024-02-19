/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

GammaCorrectionLocal::GammaCorrectionLocal(std::string id)
    : ControlNode(id), Filter(id)
{
  LOG_DEBUG("GammaCorrectionLocal::GammaCorrectionLocal()");
  this->node_type = "GammaCorrectionLocal";
  this->category = category_mapping.at(this->node_type);

  this->attr["ir"] = NEW_ATTR_INT(8, 1, 128);
  this->attr["gamma"] = NEW_ATTR_FLOAT(1.f, 0.01f, 10.f);
  this->attr["k"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
}

void GammaCorrectionLocal::compute_filter(hmap::HeightMap &h,
                                          hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  float hmin = h.min();
  float hmax = h.max();
  h.remap(0.f, 1.f, hmin, hmax);
  hmap::transform(h,
                  p_mask,
                  [this](hmap::Array &x, hmap::Array *p_mask)
                  {
                    hmap::gamma_correction_local(x,
                                                 GET_ATTR_FLOAT("gamma"),
                                                 GET_ATTR_INT("ir"),
                                                 p_mask,
                                                 GET_ATTR_FLOAT("k"));
                  });
  h.remap(hmin, hmax, 0.f, 1.f);
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
