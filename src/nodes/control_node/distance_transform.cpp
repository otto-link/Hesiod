/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

DistanceTransform::DistanceTransform(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("DistanceTransform::DistanceTransform()");
  this->node_type = "DistanceTransform";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void DistanceTransform::compute()
{
  LOG_DEBUG("computing DistanceTransform node [%s]", this->id.c_str());

  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));

  // work on a copy of the input
  this->value_out = *p_input_hmap;

  // work on a subset of the data, make sure the shapes are compatible
  if (this->shape_working.x > this->value_out.shape.x ||
      this->shape_working.y > this->value_out.shape.y)
    this->shape_working = this->value_out.shape;

  hmap::Array z = this->value_out.to_array(this->shape_working);

  if (this->reverse)
  {
    make_binary(z);
    z = 1.f - z;
  }

  z = hmap::distance_transform(z);

  this->value_out.from_array_interp(z);
  this->value_out.remap(this->vmin, this->vmax);
}

void DistanceTransform::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
