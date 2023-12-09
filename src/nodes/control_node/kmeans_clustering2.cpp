/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

KmeansClustering2::KmeansClustering2(std::string id)
    : ControlNode(id), Binary(id)
{
  this->node_type = "KmeansClustering2";
  this->category = category_mapping.at(this->node_type);

  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["nclusters"] = NEW_ATTR_INT(4, 1, 16);
  this->attr["weights.x"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["weights.y"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["shape"] = NEW_ATTR_SHAPE();
  this->attr["normalize_inputs"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"seed",
                            "nclusters",
                            "weights.x",
                            "weights.y",
                            "shape",
                            "normalize_inputs"};
}

void KmeansClustering2::compute_in_out(hmap::HeightMap &h_out,
                                       hmap::HeightMap *p_h_in1,
                                       hmap::HeightMap *p_h_in2)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  GET_ATTR_REF_SHAPE("shape")->set_value_max(h_out.shape);
  hmap::Vec2<int> shape_clustering = GET_ATTR_SHAPE("shape");

  // work on a subset of the data
  hmap::Array labels;
  {
    // make sure the shapes are compatible
    if (shape_clustering.x > h_out.shape.x ||
        shape_clustering.y > h_out.shape.y)
      shape_clustering = h_out.shape;

    hmap::Array z = h_out.to_array(shape_clustering);
    hmap::Array a1 = p_h_in1->to_array(shape_clustering);
    hmap::Array a2 = p_h_in2->to_array(shape_clustering);

    if (GET_ATTR_BOOL("normalize_inputs"))
    {
      hmap::remap(a1);
      hmap::remap(a2);
    }

    hmap::Vec2<float> weights = {GET_ATTR_FLOAT("weights.x"),
                                 GET_ATTR_FLOAT("weights.y")};

    labels = hmap::kmeans_clustering2(a1,
                                      a2,
                                      GET_ATTR_INT("nclusters"),
                                      weights,
                                      GET_ATTR_SEED("seed"));
  }

  h_out.from_array_interp_nearest(labels);
}

} // namespace hesiod::cnode
