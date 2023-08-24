/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ExpandShrink::ExpandShrink(std::string id) : Filter(id)
{
  LOG_DEBUG("ExpandShrink::ExpandShrink()");
  this->node_type = "ExpandShrink";
  this->category = category_mapping.at(this->node_type);
}

void ExpandShrink::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  // kernel definition
  hmap::Array     kernel;
  hmap::Vec2<int> kernel_shape = {2 * this->ir + 1, 2 * this->ir + 1};

  switch (this->kernel)
  {
  case kernel::cone:
    kernel = hmap::cone(kernel_shape);
    break;

  case kernel::cubic_pulse:
    kernel = hmap::cubic_pulse(kernel_shape);
    break;

  case kernel::lorentzian:
    kernel = hmap::lorentzian(kernel_shape);
    break;

  case kernel::smooth_cosine:
    kernel = hmap::smooth_cosine(kernel_shape);
    break;
  }

  // core operator
  std::function<void(hmap::Array &, hmap::Array *)> lambda;

  if (this->shrink)
    lambda = [&kernel](hmap::Array &x, hmap::Array *p_mask)
    { hmap::shrink(x, kernel, p_mask); };
  else
    lambda = [&kernel](hmap::Array &x, hmap::Array *p_mask)
    { hmap::expand(x, kernel, p_mask); };

  hmap::transform(h, p_mask, lambda);
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
