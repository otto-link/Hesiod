/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Geomorphons::Geomorphons(std::string id) : ControlNode(id), Unary(id)
{
  LOG_DEBUG("Geomorphons::Geomorphons()");
  this->node_type = "Geomorphons";
  this->category = category_mapping.at(this->node_type);

  this->attr["irmin"] = NEW_ATTR_INT(1, 0, 32);
  this->attr["irmax"] = NEW_ATTR_INT(8, 0, 32);
  this->attr["epsilon"] = NEW_ATTR_FLOAT(0.001f, 0.f, 0.01f);

  this->attr_ordered_key = {"irmin", "irmax", "epsilon"};
}

void Geomorphons::compute_in_out(hmap::HeightMap &h_out,
                                 hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  h_out = *p_h_in; // copy the input

  float hmax = h_out.max();

  hmap::transform(h_out,
                  [this, &hmax](hmap::Array &x)
                  {
                    x = geomorphons(x,
                                    GET_ATTR_INT("irmin"),
                                    GET_ATTR_INT("irmax"),
                                    GET_ATTR_FLOAT("epsilon"));
                  });
}

} // namespace hesiod::cnode
