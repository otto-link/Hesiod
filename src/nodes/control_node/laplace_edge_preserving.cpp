/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

LaplaceEdgePreserving::LaplaceEdgePreserving(std::string id) : Filter(id)
{
  this->node_type = "LaplaceEdgePreserving";
  this->category = category_mapping.at(this->node_type);
}

void LaplaceEdgePreserving::compute_filter(hmap::HeightMap &h,
                                           hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  float talus = this->talus_global / (float)this->value_out.shape.x;

  hmap::transform(h,
                  p_mask,
                  [this, &talus](hmap::Array &x, hmap::Array *p_mask)
                  {
                    hmap::laplace_edge_preserving(x,
                                                  talus,
                                                  p_mask,
                                                  this->sigma,
                                                  this->iterations);
                  });
  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
