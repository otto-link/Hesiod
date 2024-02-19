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

  this->attr["ratio"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);
  this->attr["noise_ratio"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["iterations"] = NEW_ATTR_INT(1, 1, 8);
  this->attr["edge_divisions"] = NEW_ATTR_INT(10, 1, 32);

  this->attr_ordered_key = {"ratio",
                            "noise_ratio",
                            "seed",
                            "iterations",
                            "edge_divisions"};

  this->add_port(gnode::Port("path", gnode::direction::in, dtype::dPath));
  this->add_port(gnode::Port("output", gnode::direction::out, dtype::dPath));
  this->update_inner_bindings();
}

void MeanderizePath::compute()
{
  LOG_DEBUG("computing MeanderizePath node [%s]", this->id.c_str());

  hmap::Path *p_path = CAST_PORT_REF(hmap::Path, "path");

  // work on a copy of the input
  this->value_out = *p_path;

  if (p_path->get_npoints() > 1)
    this->value_out.meanderize(GET_ATTR_FLOAT("ratio"),
                               GET_ATTR_FLOAT("noise_ratio"),
                               GET_ATTR_SEED("seed"),
                               GET_ATTR_INT("iterations"),
                               GET_ATTR_INT("edge_divisions"));
}

void MeanderizePath::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
