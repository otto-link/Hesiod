/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

MeanderizePath::MeanderizePath(std::string id) : ControlNode(id)
{
  LOG_DEBUG("MeanderizePath::MeanderizePath()");
  this->node_type = "MeanderizePath";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("path", gnode::direction::in, dtype::dPath));
  this->add_port(gnode::Port("output", gnode::direction::out, dtype::dPath));
  this->update_inner_bindings();
}

void MeanderizePath::compute()
{
  LOG_DEBUG("computing MeanderizePath node [%s]", this->id.c_str());

  hmap::Path *p_input_path = static_cast<hmap::Path *>(
      (void *)this->get_p_data("path"));

  // work on a copy of the input
  this->value_out = *p_input_path;

  if (p_input_path->get_npoints() > 1)
    this->value_out.meanderize(this->radius,
                               this->tangent_contribution,
                               this->iterations,
                               this->transition_length_ratio);
}

void MeanderizePath::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
