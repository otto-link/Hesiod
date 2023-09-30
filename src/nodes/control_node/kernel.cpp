/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Kernel::Kernel(std::string id) : gnode::Node(id)
{
  this->node_type = "Kernel";
  this->category = category_mapping.at(this->node_type);

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

  switch (this->kernel)
  {
  case kernel::cone:
    this->value_out = hmap::cone(this->shape);
    break;

  case kernel::cubic_pulse:
    this->value_out = hmap::cubic_pulse(this->shape);
    break;

  case kernel::lorentzian:
    this->value_out = hmap::lorentzian(this->shape);
    break;

  case kernel::smooth_cosine:
    this->value_out = hmap::smooth_cosine(this->shape);
    break;
  }

  if (this->normalized)
    this->value_out.normalize();
  else
    hmap::remap(value_out, this->vmin, this->vmax);
}

} // namespace hesiod::cnode
