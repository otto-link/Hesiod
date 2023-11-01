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

  // work on a subset of the data
  hmap::Array labels;
  {
    // make sure the shapes are compatible
    if (this->shape_clustering.x > h_out.shape.x ||
        this->shape_clustering.y > h_out.shape.y)
      this->shape_clustering = h_out.shape;

    hmap::Array z = h_out.to_array(this->shape_clustering);
    hmap::Array a1 = p_h_in1->to_array(this->shape_clustering);
    hmap::Array a2 = p_h_in2->to_array(this->shape_clustering);

    if (this->normalize_inputs)
    {
      hmap::remap(a1);
      hmap::remap(a2);
    }

    labels = hmap::kmeans_clustering2(a1,
                                      a2,
                                      this->nclusters,
                                      this->weights,
                                      (uint)this->seed);
  }

  h_out.from_array_interp_nearest(labels);
}

} // namespace hesiod::cnode
