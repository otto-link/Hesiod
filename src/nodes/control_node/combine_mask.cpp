/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

CombineMask::CombineMask(std::string id) : ControlNode(id)
{
  LOG_DEBUG("CombineMask::CombineMask()");
  this->node_type = "CombineMask";
  this->category = category_mapping.at(this->node_type);
  this->attr["method"] = NEW_ATTR_MAPENUM(this->method_map);

  this->add_port(
      gnode::Port("input 1", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("input 2", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void CombineMask::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

void CombineMask::compute()
{
  LOG_DEBUG("computing CombineMask node [%s]", this->id.c_str());

  hmap::HeightMap *p_in1 = CAST_PORT_REF(hmap::HeightMap, "input 1");
  hmap::HeightMap *p_in2 = CAST_PORT_REF(hmap::HeightMap, "input 2");

  this->value_out.set_sto(p_in1->shape, p_in1->tiling, p_in1->overlap);

  std::function<void(hmap::Array &, hmap::Array &, hmap::Array &)> lambda;

  int method = GET_ATTR_MAPENUM("method");

  switch (method)
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

} // namespace hesiod::cnode
