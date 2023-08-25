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
}

void Blend::compute_in_out(hmap::HeightMap &h_out,
                           hmap::HeightMap *p_h_in1,
                           hmap::HeightMap *p_h_in2)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  std::function<void(hmap::Array &, hmap::Array &, hmap::Array &)> lambda;

  switch (this->method)
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
    lambda = [this](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::blend_gradients(a1, a2, this->ir); };
    break;

  case blending_method::maximum:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::maximum(a1, a2); };
    break;

  case blending_method::maximum_smooth:
    lambda = [this](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::maximum_smooth(a1, a2, this->k); };
    break;

  case blending_method::minimum:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::minimum(a1, a2); };
    break;

  case blending_method::minimum_smooth:
    lambda = [this](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::minimum_smooth(a1, a2, this->k); };
    break;

  case blending_method::multiply:
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = a1 * a2; };
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
}

} // namespace hesiod::cnode
