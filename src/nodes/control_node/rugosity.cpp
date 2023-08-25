/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Rugosity::Rugosity(std::string id) : Unary(id)
{
  this->node_type = "Rugosity";
  this->category = category_mapping.at(this->node_type);
}

void Rugosity::compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  // make a copy of the input and applied range remapping
  hmap::transform(h_out,
                  *p_h_in,
                  [this](hmap::Array &out, hmap::Array &in)
                  { out = hmap::rugosity(in, this->ir); });
  h_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode