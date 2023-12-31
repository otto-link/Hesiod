/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Median3x3::Median3x3(std::string id) : ControlNode(id), Filter(id)
{
  LOG_DEBUG("Median3x3::Median3x3()");
  this->node_type = "Median3x3";
  this->category = category_mapping.at(this->node_type);
}

void Median3x3::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  hmap::transform(h,
                  p_mask,
                  [](hmap::Array &x, hmap::Array *p_mask)
                  { hmap::median_3x3(x, p_mask); });
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
