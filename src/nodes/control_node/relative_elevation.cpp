/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

RelativeElevation::RelativeElevation(std::string id) : Unary(id)
{
  LOG_DEBUG("RelativeElevation::RelativeElevation()");
  this->node_type = "RelativeElevation";
  this->category = category_mapping.at(this->node_type);
}

void RelativeElevation::compute_in_out(hmap::HeightMap &h_out,
                                       hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  h_out = *p_h_in; // copy the input

  hmap::transform(h_out,
                  [this](hmap::Array &x)
                  { x = hmap::relative_elevation(x, this->ir); });

  h_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode