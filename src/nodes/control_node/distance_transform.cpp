/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

DistanceTransform::DistanceTransform(std::string id) : ControlNode(id)
{
  LOG_DEBUG("DistanceTransform::DistanceTransform, id: %s", id.c_str());
  this->node_type = "DistanceTransform";
  this->category = category_mapping.at(this->node_type);

  this->attr["shape"] = NEW_ATTR_SHAPE();
  this->attr["reverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_RANGE(true);

  this->attr_ordered_key = {"shape", "reverse", "remap"};

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void DistanceTransform::compute()
{
  LOG_DEBUG("computing DistanceTransform node [%s]", this->id.c_str());

  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");

  // work on a copy of the input
  this->value_out = *p_hmap;

  GET_ATTR_REF_SHAPE("shape")->set_value_max(this->value_out.shape);
  hmap::Vec2<int> shape_working = GET_ATTR_SHAPE("shape");

  // work on a subset of the data, make sure the shapes are compatible
  if (shape_working.x > this->value_out.shape.x ||
      shape_working.y > this->value_out.shape.y)
    shape_working = this->value_out.shape;

  hmap::Array z = this->value_out.to_array(shape_working);

  if (GET_ATTR_BOOL("reverse"))
  {
    make_binary(z);
    z = 1.f - z;
  }

  z = hmap::distance_transform(z);

  this->value_out.from_array_interp(z);
  this->post_process_heightmap(this->value_out);
}

void DistanceTransform::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
