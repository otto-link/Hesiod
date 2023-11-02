/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

KmeansClustering3::KmeansClustering3(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("KmeansClustering3::KmeansClustering3()");
  this->add_port(
      gnode::Port("input##1", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("input##2", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("input##3", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
  this->node_type = "KmeansClustering3";
  this->category = category_mapping.at(this->node_type);
}

void KmeansClustering3::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void KmeansClustering3::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input1 = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input##1"));
  hmap::HeightMap *p_input2 = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input##2"));
  hmap::HeightMap *p_input3 = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input##3"));

  this->value_out.set_sto(p_input1->shape, p_input1->tiling, p_input1->overlap);

  // work on a subset of the data
  hmap::Array labels;
  {
    // make sure the shapes are compatible
    if (this->shape_clustering.x > this->value_out.shape.x ||
        this->shape_clustering.y > this->value_out.shape.y)
      this->shape_clustering = this->value_out.shape;

    hmap::Array z = this->value_out.to_array(this->shape_clustering);
    hmap::Array a1 = p_input1->to_array(this->shape_clustering);
    hmap::Array a2 = p_input2->to_array(this->shape_clustering);
    hmap::Array a3 = p_input3->to_array(this->shape_clustering);

    if (this->normalize_inputs)
    {
      hmap::remap(a1);
      hmap::remap(a2);
      hmap::remap(a3);
    }

    labels = hmap::kmeans_clustering3(a1,
                                      a2,
                                      a3,
                                      this->nclusters,
                                      this->weights,
                                      (uint)this->seed);
  }

  this->value_out.from_array_interp_nearest(labels);
}

} // namespace hesiod::cnode
