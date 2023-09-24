/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Equalize::Equalize(std::string id) : Filter(id)
{
  this->node_type = "Equalize";
  this->category = category_mapping.at(this->node_type);
}

void Equalize::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());
  hmap::transform(h,
                  p_mask,
                  [this](hmap::Array &x, hmap::Array *p_mask)
                  { hmap::equalize(x, p_mask); });
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
