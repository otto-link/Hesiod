/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

DigPath::DigPath(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("DigPath::DigPath()");
  this->node_type = "DigPath";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("path", gnode::direction::in, dtype::dPath));
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("output", gnode::direction::out, dtype::dPath));
  this->update_inner_bindings();
}

void DigPath::compute()
{
  LOG_DEBUG("computing DigPath node [%s]", this->id.c_str());

  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));
  hmap::Path *p_input_path = static_cast<hmap::Path *>(
      (void *)this->get_p_data("path"));

  if (p_input_path->get_npoints() > 1)
  {
    // work on a copy of the input
    this->value_out = *p_input_hmap;

    hmap::transform(this->value_out,
                    [this, p_input_path](hmap::Array &z, hmap::Vec4<float> bbox)
                    {
                      hmap::dig_path(z,
                                     *p_input_path,
                                     this->width,
                                     this->decay,
                                     this->flattening_radius,
                                     bbox);
                    });

    this->value_out.smooth_overlap_buffers();
  }
}

void DigPath::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
