/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

CombineMask::CombineMask(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("CombineMask::CombineMask()");
  this->node_type = "CombineMask";
  this->category = category_mapping.at(this->node_type);
  this->add_port(
      gnode::Port("input 1", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("input 2", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void CombineMask::compute()
{
  LOG_DEBUG("computing CombineMask node [%s]", this->id.c_str());

  hmap::HeightMap *p_in1 = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input 1"));
  hmap::HeightMap *p_in2 = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input 2"));

  this->value_out.set_sto(p_in1->shape, p_in1->tiling, p_in1->overlap);

  std::function<void(hmap::Array &, hmap::Array &, hmap::Array &)> lambda;

  switch (this->method)
  {
  case 0: // union
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::maximum(a1, a2); };
    break;

  case 1: // intersection
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    { m = hmap::minimum(a1, a2); };
    break;

  case 2: // exclusion
    lambda = [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
    {
      m = a1 - a2;
      hmap::clamp_min(m, 0.f);
    };
    break;
  }

  hmap::transform(this->value_out, *p_in1, *p_in2, lambda);
}

void CombineMask::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
