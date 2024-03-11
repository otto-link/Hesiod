/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Wrinkle::Wrinkle(std::string id) : ControlNode(id), Filter(id)
{
  this->node_type = "Wrinkle";
  this->category = category_mapping.at(this->node_type);

  this->attr["wrinkle_amplitude"] = NEW_ATTR_FLOAT(0.1f, 0.f, 0.5f);
  this->attr["displacement_amplitude"] = NEW_ATTR_FLOAT(1.f, 0.f, 2.f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.f, 0.f, 1.f);
  this->attr["kw"] = NEW_ATTR_FLOAT(2.f, 0.01f, 64.f);
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["octaves"] = NEW_ATTR_INT(8, 1, 128);
  this->attr["weight"] = NEW_ATTR_FLOAT(0.7f, 0.f, 1.f);

  this->attr_ordered_key = {"wrinkle_amplitude",
                            "displacement_amplitude",
                            "_radius",
                            "_kw",
                            "_seed",
                            "_octaves",
                            "_weight"};
}

void Wrinkle::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  // "de-normalize" amplitude (1e2f is arbitrary)
  float amp = GET_ATTR_FLOAT("wrinkle_amplitude") * (float)h.shape.x / 1e2f;
  int   ir = (int)(GET_ATTR_FLOAT("radius") * h.shape.x);

  hmap::transform(h,
                  p_mask,
                  [this, &amp, &ir](hmap::Array &x, hmap::Array *p_mask)
                  {
                    hmap::wrinkle(x,
                                  amp,
                                  p_mask,
                                  GET_ATTR_FLOAT("displacement_amplitude"),
                                  ir,
                                  GET_ATTR_FLOAT("kw"),
                                  GET_ATTR_SEED("seed"),
                                  GET_ATTR_INT("octaves"),
                                  GET_ATTR_FLOAT("weight"));
                  });

  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
