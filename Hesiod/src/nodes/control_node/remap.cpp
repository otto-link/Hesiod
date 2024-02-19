/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Remap::Remap(std::string id) : ControlNode(id), Unary(id)
{
  this->node_type = "Remap";
  this->category = category_mapping.at(this->node_type);

  this->attr["remap"] = NEW_ATTR_RANGE(true);
}

void Remap::compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  // make a copy of the input and applied range remapping
  h_out = *p_h_in;
  h_out.remap(GET_ATTR_RANGE("remap").x, GET_ATTR_RANGE("remap").y);
}

} // namespace hesiod::cnode
