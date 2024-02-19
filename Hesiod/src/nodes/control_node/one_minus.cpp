/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

OneMinus::OneMinus(std::string id) : ControlNode(id), Unary(id)
{
  LOG_DEBUG("OneMinus::OneMinus()");
  this->node_type = "OneMinus";
  this->category = category_mapping.at(this->node_type);
  this->attr["remap"] = NEW_ATTR_RANGE(false);
}

void OneMinus::compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  h_out = *p_h_in; // copy the input

  float hmax = h_out.max();

  hmap::transform(h_out, [this, &hmax](hmap::Array &x) { x = hmax - x; });
}

} // namespace hesiod::cnode
