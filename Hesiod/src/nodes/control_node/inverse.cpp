/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Inverse::Inverse(std::string id) : ControlNode(id), Unary(id)
{
  LOG_DEBUG("Inverse::Inverse()");
  this->node_type = "Inverse";
  this->category = category_mapping.at(this->node_type);
  this->attr["remap"] = NEW_ATTR_RANGE(false);
}

void Inverse::compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  h_out = *p_h_in; // copy the input

  float hmax = h_out.max();

  hmap::transform(h_out, [this, &hmax](hmap::Array &x) { x *= -1.f; });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
