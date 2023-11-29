/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

MeanLocal::MeanLocal(std::string id) : Filter(id)
{
  LOG_DEBUG("MeanLocal::MeanLocal()");
  this->node_type = "MeanLocal";
  this->category = category_mapping.at(this->node_type);
  this->attr["ir"] = NEW_ATTR_INT(8, 1, 128);
}

void MeanLocal::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  // TODO add mask

  hmap::transform(h,
                  p_mask,
                  [this](hmap::Array &x, hmap::Array *p_mask)
                  { x = hmap::mean_local(x, GET_ATTR_INT("ir")); });
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
