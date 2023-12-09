/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Abs::Abs(std::string id) : ControlNode(id), Unary(id)
{
  LOG_DEBUG("Abs::Abs()");
  this->node_type = "Abs";
  this->category = category_mapping.at(this->node_type);
}

void Abs::compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  h_out = *p_h_in; // copy the input

  hmap::transform(h_out, [this](hmap::Array &x) { x = hmap::abs(x); });
}

} // namespace hesiod::cnode
