/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

PathFinding::PathFinding(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("PathFinding::PathFinding()");
  this->node_type = "PathFinding";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("path", gnode::direction::in, dtype::dPath));
  this->add_port(
      gnode::Port("heightmap", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("mask nogo",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("output", gnode::direction::out, dtype::dPath));
  this->update_inner_bindings();
}

void PathFinding::compute()
{
  LOG_DEBUG("computing PathFinding node [%s]", this->id.c_str());

  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("heightmap"));
  hmap::HeightMap *p_mask = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("mask nogo"));
  hmap::Path *p_input_path = static_cast<hmap::Path *>(
      (void *)this->get_p_data("path"));

  // work on a copy of the input
  this->value_out = *p_input_path;

  if (p_input_path->get_npoints() > 1)
  // work on a subset of the data
  {
    if (this->wshape.x > (*p_input_hmap).shape.x ||
        this->wshape.y > (*p_input_hmap).shape.y)
      this->wshape = (*p_input_hmap).shape;

    hmap::Array z = (*p_input_hmap).to_array(this->wshape);
    hmap::remap(z);

    hmap::Array *p_mask_array = nullptr;
    hmap::Array  mask_array;

    if (p_mask)
    {
      mask_array = p_mask->to_array(this->wshape);
      p_mask_array = &mask_array;
    }

    this->value_out.dijkstra(z,
                             {0.f, 1.f, 0.f, 1.f},
                             0,
                             this->elevation_ratio,
                             this->distance_exponent,
                             p_mask_array);
  }
}

void PathFinding::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
