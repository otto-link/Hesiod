/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Clamp::Clamp(std::string id) : Unary(id)
{
  LOG_DEBUG("Clamp::Clamp()");
  this->node_type = "Clamp";
  this->category = category_mapping.at(this->node_type);

  this->attr["clamp"] = NEW_ATTR_RANGE(true);
  this->attr["smooth_min"] = NEW_ATTR_BOOL(false);
  this->attr["k_min"] = NEW_ATTR_FLOAT(0.05f, 0.f, 1.f);
  this->attr["smooth_max"] = NEW_ATTR_BOOL(false);
  this->attr["k_max"] = NEW_ATTR_FLOAT(0.05f, 0.f, 1.f);

  this->attr_ordered_key = {"clamp",
                            "smooth_min",
                            "_k_min",
                            "smooth_max",
                            "_k_max"};
}

void Clamp::compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  h_out = *p_h_in; // copy the input

  // retrieve parameters
  hmap::Vec2<float> crange = GET_ATTR_RANGE("clamp");
  bool              smooth_min = GET_ATTR_BOOL("smooth_min");
  bool              smooth_max = GET_ATTR_BOOL("smooth_max");
  float             k_min = GET_ATTR_FLOAT("k_min");
  float             k_max = GET_ATTR_FLOAT("k_max");

  // compute
  if (GET_ATTR_REF_RANGE("clamp")->is_activated())
  {
    if (!smooth_min && !smooth_max)
    {
      hmap::transform(h_out,
                      [&crange](hmap::Array &x)
                      { hmap::clamp(x, crange.x, crange.y); });
    }
    else
    {
      if (smooth_min)
        hmap::transform(h_out,
                        [&crange, &k_min](hmap::Array &x)
                        { hmap::clamp_min_smooth(x, crange.x, k_min); });
      else
        hmap::transform(h_out,
                        [&crange](hmap::Array &x)
                        { hmap::clamp_min(x, crange.x); });

      if (smooth_max)
        hmap::transform(h_out,
                        [&crange, &k_max](hmap::Array &x)
                        { hmap::clamp_max_smooth(x, crange.y, k_max); });
      else
        hmap::transform(h_out,
                        [&crange](hmap::Array &x)
                        { hmap::clamp_max(x, crange.y); });
    }
  }
}

} // namespace hesiod::cnode
