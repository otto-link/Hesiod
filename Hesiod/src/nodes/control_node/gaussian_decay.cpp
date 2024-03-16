/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

GaussianDecay::GaussianDecay(std::string id) : ControlNode(id), Unary(id)
{
  LOG_DEBUG("GaussianDecay::GaussianDecay()");
  this->node_type = "GaussianDecay";
  this->category = category_mapping.at(this->node_type);

  this->attr["sigma"] = NEW_ATTR_FLOAT(0.1f, 0.f, 2.f);
}

void GaussianDecay::compute_in_out(hmap::HeightMap &h_out,
                                   hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  h_out = *p_h_in; // copy the input

  hmap::transform(h_out,
                  [this](hmap::Array &x)
                  { x = hmap::gaussian_decay(x, GET_ATTR_FLOAT("sigma")); });
}

} // namespace hesiod::cnode
