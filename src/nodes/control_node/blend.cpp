/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Blend::Blend(std::string id) : Binary(id)
{
  this->node_type = "Blend";
  this->category = category_mapping.at(this->node_type);

  this->attr["blending_method"] = NEW_ATTR_MAPENUM(this->blending_method_map);
  this->attr["k"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 1.f);
  this->attr["ir"] = NEW_ATTR_INT(4, 1, 128);
  this->attr_ordered_key = {"blending_method", "k", "ir"};
}

void Blend::compute_in_out(hmap::HeightMap &h_out,
                           hmap::HeightMap *p_h_in1,
                           hmap::HeightMap *p_h_in2)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  std::function<void(hmap::Array &, hmap::Array &, hmap::Array &)> lambda;

  int   method = GET_ATTR_MAPENUM("blending_method");
  float k = GET_ATTR_FLOAT("k");
  int   ir = GET_ATTR_INT("ir");

  switch (method)
  {
  case blending_method::add:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = a1 + a2; };
    break;

  case blending_method::exclusion:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::blend_exclusion(a1, a2); };
    break;

  case blending_method::gradients:
    lambda = [&ir](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::blend_gradients(a1, a2, ir); };
    break;

  case blending_method::maximum:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::maximum(a1, a2); };
    break;

  case blending_method::maximum_smooth:
    lambda = [&k](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::maximum_smooth(a1, a2, k); };
    break;

  case blending_method::minimum:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::minimum(a1, a2); };
    break;

  case blending_method::minimum_smooth:
    lambda = [&k](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::minimum_smooth(a1, a2, k); };
    break;

  case blending_method::multiply:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = a1 * a2; };
    break;

  case blending_method::multiply_add:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = a1 + a1 * a2; };
    break;

  case blending_method::negate:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::blend_negate(a1, a2); };
    break;

  case blending_method::overlay:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::blend_overlay(a1, a2); };
    break;

  case blending_method::soft:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::blend_soft(a1, a2); };
    break;

  case blending_method::substract:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = a1 - a2; };
    break;
  }

  hmap::transform(h_out, *p_h_in1, *p_h_in2, lambda);

  if (method == blending_method::gradients)
    h_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
