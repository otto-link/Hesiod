/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ExpandShrinkDirectional::ExpandShrinkDirectional(std::string id) : Filter(id)
{
  LOG_DEBUG("ExpandShrinkDirectional::ExpandShrinkDirectional()");
  this->node_type = "ExpandShrinkDirectional";
  this->category = category_mapping.at(this->node_type);
}

void ExpandShrinkDirectional::compute_filter(hmap::HeightMap &h,
                                             hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  // core operator
  std::function<void(hmap::Array &, hmap::Array *)> lambda;

  if (this->shrink)
    lambda = [this](hmap::Array &x, hmap::Array *p_mask)
    {
      hmap::shrink_directional(x,
                               this->ir,
                               this->angle,
                               this->aspect_ratio,
                               this->anisotropy,
                               p_mask);
    };
  else
    lambda = [this](hmap::Array &x, hmap::Array *p_mask)
    {
      hmap::expand_directional(x,
                               this->ir,
                               this->angle,
                               this->aspect_ratio,
                               this->anisotropy,
                               p_mask);
    };

  hmap::transform(h, p_mask, lambda);
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
