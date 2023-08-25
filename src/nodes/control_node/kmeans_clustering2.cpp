/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

KmeansClustering2::KmeansClustering2(std::string id) : Binary(id)
{
  this->node_type = "KmeansClustering2";
  this->category = category_mapping.at(this->node_type);
}

void KmeansClustering2::compute_in_out(hmap::HeightMap &h_out,
                                       hmap::HeightMap *p_h_in1,
                                       hmap::HeightMap *p_h_in2)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  auto lambda = [this](hmap::Array &m, hmap::Array &a1, hmap::Array &a2)
  { m = kmeans_clustering2(a1, a2, this->nclusters, (uint)this->seed); };

  hmap::transform(h_out, *p_h_in1, *p_h_in2, lambda);
}

} // namespace hesiod::cnode
