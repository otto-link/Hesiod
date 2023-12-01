/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Kernel::Kernel(std::string id) : ControlNode(id)
{
  this->node_type = "Kernel";
  this->category = category_mapping.at(this->node_type);

  this->attr["kernel"] = NEW_ATTR_MAPENUM(this->kernel_map);
  this->attr["ir"] = NEW_ATTR_INT(8, 1, 128);
  this->attr["remap"] = NEW_ATTR_RANGE(true);

  this->attr_ordered_key = {"kernel", "ir", "remap"};

  this->add_port(gnode::Port("output", gnode::direction::out, dtype::dArray));
  this->update_inner_bindings();
}

void Kernel::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void Kernel::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::Vec2<int> shape = hmap::Vec2<int>(2 * GET_ATTR_INT("ir"),
                                          2 * GET_ATTR_INT("ir"));

  switch (GET_ATTR_MAPENUM("kernel"))
  {
  case kernel::cone:
    this->value_out = hmap::cone(shape);
    break;

  case kernel::cubic_pulse:
    this->value_out = hmap::cubic_pulse(shape);
    break;

  case kernel::lorentzian:
    this->value_out = hmap::lorentzian(shape);
    break;

  case kernel::smooth_cosine:
    this->value_out = hmap::smooth_cosine(shape);
    break;
  }

  if (GET_ATTR_REF_RANGE("remap")->is_activated())
    this->value_out.normalize();
  else
    hmap::remap(value_out,
                GET_ATTR_RANGE("remap").x,
                GET_ATTR_RANGE("remap").y);
}

} // namespace hesiod::cnode
