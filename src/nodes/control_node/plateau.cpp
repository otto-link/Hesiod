/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Plateau::Plateau(std::string id) : Filter(id)
{
  this->node_type = "Plateau";
  this->category = category_mapping.at(this->node_type);

  this->attr["ir"] = NEW_ATTR_INT(32, 1, 256);
  this->attr["factor"] = NEW_ATTR_FLOAT(4.f, 0.01f, 10.f);

  this->attr_ordered_key = {"ir", "factor"};
}

void Plateau::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());
  hmap::transform(
      h,
      p_mask,
      [this](hmap::Array &x, hmap::Array *p_mask) {
        hmap::plateau(x, p_mask, GET_ATTR_INT("ir"), GET_ATTR_FLOAT("factor"));
      });
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
