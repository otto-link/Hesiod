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

  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 1.f);
  this->attr["tangent_contribution"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["iterations"] = NEW_ATTR_INT(1, 1, 10);
  this->attr["transition_length_ratio"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);

  this->attr_ordered_key = {"radius",
                            "tangent_contribution",
                            "iterations",
                            "transition_length_ratio"};

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
    this->value_out.meanderize(GET_ATTR_FLOAT("radius"),
                               GET_ATTR_FLOAT("tangent_contribution"),
                               GET_ATTR_INT("iterations"),
                               GET_ATTR_FLOAT("transition_length_ratio"));
}

void MeanderizePath::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
