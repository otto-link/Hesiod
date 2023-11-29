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

  this->attr["kernel"] = NEW_ATTR_MAPENUM(this->kernel_map);
  this->attr["ir"] = NEW_ATTR_INT(4, 1, 128);
  this->attr["shrink"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"kernel", "ir", "shrink"};
}

void ExpandShrink::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  // kernel definition
  hmap::Array     kernel_array;
  hmap::Vec2<int> kernel_shape = {2 * GET_ATTR_INT("ir") + 1,
                                  2 * GET_ATTR_INT("ir") + 1};

  switch (GET_ATTR_MAPENUM("kernel"))
  {
  case kernel::cone:
    kernel_array = hmap::cone(kernel_shape);
    break;

  case kernel::cubic_pulse:
    kernel_array = hmap::cubic_pulse(kernel_shape);
    break;

  case kernel::lorentzian:
    kernel_array = hmap::lorentzian(kernel_shape);
    break;

  case kernel::smooth_cosine:
    kernel_array = hmap::smooth_cosine(kernel_shape);
    break;
  }

  // core operator
  std::function<void(hmap::Array &, hmap::Array *)> lambda;

  if (GET_ATTR_BOOL("shrink"))
    lambda = [&kernel_array](hmap::Array &x, hmap::Array *p_mask)
    { hmap::shrink(x, kernel_array, p_mask); };
  else
    lambda = [&kernel_array](hmap::Array &x, hmap::Array *p_mask)
    { hmap::expand(x, kernel_array, p_mask); };

  hmap::transform(h, p_mask, lambda);
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
