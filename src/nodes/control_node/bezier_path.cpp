/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

BezierPath::BezierPath(std::string id) : ControlNode(id)
{
  LOG_DEBUG("BezierPath::BezierPath()");
  this->node_type = "BezierPath";
  this->category = category_mapping.at(this->node_type);

  this->attr["curvature_ratio"] = NEW_ATTR_FLOAT(0.3f, 0.f, 1.f);
  this->attr["edge_divisions"] = NEW_ATTR_INT(10, 1, 32);

  this->add_port(gnode::Port("path", gnode::direction::in, dtype::dPath));
  this->add_port(gnode::Port("output", gnode::direction::out, dtype::dPath));
  this->update_inner_bindings();
}

void BezierPath::compute()
{
  LOG_DEBUG("computing BezierPath node [%s]", this->id.c_str());

  hmap::Path *p_input_path = static_cast<hmap::Path *>(
      (void *)this->get_p_data("path"));

  // work on a copy of the input
  this->value_out = *p_input_path;

  if (p_input_path->get_npoints() > 1)
    this->value_out.bezier(GET_ATTR_FLOAT("curvature_ratio"),
                           GET_ATTR_INT("edge_divisions"));
}

void BezierPath::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
