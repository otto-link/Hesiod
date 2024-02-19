/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

KmeansClustering3::KmeansClustering3(std::string id) : ControlNode(id)
{
  LOG_DEBUG("KmeansClustering3::KmeansClustering3()");
  this->node_type = "KmeansClustering3";
  this->category = category_mapping.at(this->node_type);

  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["nclusters"] = NEW_ATTR_INT(4, 1, 16);
  this->attr["weights.x"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["weights.y"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["weights.z"] = NEW_ATTR_FLOAT(1.f, 0.01f, 2.f);
  this->attr["shape"] = NEW_ATTR_SHAPE();
  this->attr["normalize_inputs"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"seed",
                            "nclusters",
                            "weights.x",
                            "weights.y",
                            "weights.z",
                            "shape",
                            "normalize_inputs"};

  this->add_port(
      gnode::Port("input##1", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("input##2", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("input##3", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void KmeansClustering3::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void KmeansClustering3::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::HeightMap *p_input1 = CAST_PORT_REF(hmap::HeightMap, "input##1");
  hmap::HeightMap *p_input2 = CAST_PORT_REF(hmap::HeightMap, "input##2");
  hmap::HeightMap *p_input3 = CAST_PORT_REF(hmap::HeightMap, "input##3");

  this->value_out.set_sto(p_input1->shape, p_input1->tiling, p_input1->overlap);

  GET_ATTR_REF_SHAPE("shape")->set_value_max(this->value_out.shape);
  hmap::Vec2<int> shape_clustering = GET_ATTR_SHAPE("shape");

  // work on a subset of the data
  hmap::Array labels;
  {
    // make sure the shapes are compatible
    if (shape_clustering.x > this->value_out.shape.x ||
        shape_clustering.y > this->value_out.shape.y)
      shape_clustering = this->value_out.shape;

    hmap::Array z = this->value_out.to_array(shape_clustering);
    hmap::Array a1 = p_input1->to_array(shape_clustering);
    hmap::Array a2 = p_input2->to_array(shape_clustering);
    hmap::Array a3 = p_input3->to_array(shape_clustering);

    if (GET_ATTR_BOOL("normalize_inputs"))
    {
      hmap::remap(a1);
      hmap::remap(a2);
      hmap::remap(a3);
    }

    hmap::Vec3<float> weights = {GET_ATTR_FLOAT("weights.x"),
                                 GET_ATTR_FLOAT("weights.y"),
                                 GET_ATTR_FLOAT("weights.z")};

    labels = hmap::kmeans_clustering3(a1,
                                      a2,
                                      a3,
                                      GET_ATTR_INT("nclusters"),
                                      weights,
                                      GET_ATTR_SEED("seed"));
  }

  this->value_out.from_array_interp_nearest(labels);
}

} // namespace hesiod::cnode
