/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

MinimumLocal::MinimumLocal(std::string id) : Unary(id)
{
  LOG_DEBUG("MinimumLocal::MinimumLocal()");
  this->node_type = "MinimumLocal";
  this->category = category_mapping.at(this->node_type);
  this->attr["ir"] = NEW_ATTR_INT(8, 1, 128);
}

void MinimumLocal::compute_in_out(hmap::HeightMap &h_out,
                                  hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  // make a copy of the input and applied range remapping
  hmap::transform(h_out,
                  *p_h_in,
                  [this](hmap::Array &x, hmap::Array &y)
                  { x = hmap::minimum_local(y, GET_ATTR_INT("ir")); });
  h_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
