/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

CubicPulseTruncated::CubicPulseTruncated(std::string id) : gnode::Node(id)
{
  this->node_type = "CubicPulseTruncated";
  this->category = category_mapping.at(this->node_type);

  this->add_port(gnode::Port("output", gnode::direction::out, dtype::dArray));
  this->update_inner_bindings();
}

void CubicPulseTruncated::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void CubicPulseTruncated::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  this->value_out = hmap::cubic_pulse_truncated(this->shape,
                                                this->slant_ratio,
                                                this->angle);

  if (this->normalized)
    this->value_out.normalize();
  else
    hmap::remap(value_out, this->vmin, this->vmax);
}

} // namespace hesiod::cnode
