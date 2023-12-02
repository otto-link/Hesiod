/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ToKernel::ToKernel(std::string id) : ControlNode(id)
{
  this->node_type = "ToKernel";
  this->category = category_mapping.at(this->node_type);

  this->attr["ir"] = NEW_ATTR_INT(8, 1, 128);
  this->attr["remap"] = NEW_ATTR_RANGE(true);

  this->attr_ordered_key = {"ir", "remap"};

  this->add_port(
      gnode::Port("heightmap", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("kernel", gnode::direction::out, dtype::dArray));

  hmap::Vec2<int> shape = hmap::Vec2<int>(2 * GET_ATTR_INT("ir"),
                                          2 * GET_ATTR_INT("ir"));
  this->value_out = hmap::Array(shape);

  this->update_inner_bindings();
}

void ToKernel::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("kernel", (void *)&(this->value_out));
}

void ToKernel::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "heightmap");

  hmap::Vec2<int> shape = hmap::Vec2<int>(2 * GET_ATTR_INT("ir"),
                                          2 * GET_ATTR_INT("ir"));

  // TODO use a distributed procedure instead of this
  this->value_out = p_hmap->to_array().resample_to_shape(shape);

  if (GET_ATTR_REF_RANGE("remap")->is_activated())
    this->value_out.normalize();
  else
    hmap::remap(this->value_out,
                GET_ATTR_RANGE("remap").x,
                GET_ATTR_RANGE("remap").y);
}

} // namespace hesiod::cnode
