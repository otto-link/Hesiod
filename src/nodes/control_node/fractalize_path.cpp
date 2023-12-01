/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

FractalizePath::FractalizePath(std::string id) : ControlNode(id)
{
  LOG_DEBUG("FractalizePath::FractalizePath()");
  this->node_type = "FractalizePath";
  this->category = category_mapping.at(this->node_type);

  this->attr["iterations"] = NEW_ATTR_INT(1, 1, 10);
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["sigma"] = NEW_ATTR_FLOAT(0.3f, 0.f, 1.f);
  this->attr["orientation"] = NEW_ATTR_INT(0, 0, 1);
  this->attr["persistence"] = NEW_ATTR_FLOAT(1.f, 0.01f, 4.f);

  this->attr_ordered_key = {"iterations",
                            "seed",
                            "sigma",
                            "orientation",
                            "persistence"};

  this->add_port(gnode::Port("path", gnode::direction::in, dtype::dPath));
  this->add_port(gnode::Port("output", gnode::direction::out, dtype::dPath));
  this->update_inner_bindings();
}

void FractalizePath::compute()
{
  LOG_DEBUG("computing FractalizePath node [%s]", this->id.c_str());

  hmap::Path *p_input_path = static_cast<hmap::Path *>(
      (void *)this->get_p_data("path"));

  // work on a copy of the input
  this->value_out = *p_input_path;

  if (p_input_path->get_npoints() > 1)
    this->value_out.fractalize(GET_ATTR_INT("iterations"),
                               GET_ATTR_SEED("seed"),
                               GET_ATTR_FLOAT("sigma"),
                               GET_ATTR_INT("orientation"),
                               GET_ATTR_FLOAT("persistence"));
}

void FractalizePath::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
