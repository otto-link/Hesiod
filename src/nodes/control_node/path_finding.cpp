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
  this->add_port(gnode::Port("output", gnode::direction::out, dtype::dPath));
  this->update_inner_bindings();
}

void PathFinding::compute()
{
  LOG_DEBUG("computing PathFinding node [%s]", this->id.c_str());

  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("heightmap"));
  hmap::Path *p_input_path = static_cast<hmap::Path *>(
      (void *)this->get_p_data("path"));

  // work on a copy of the input
  this->value_out = *p_input_path;

  if (p_input_path->get_npoints() > 1)
  {
    // work on a subset of the data
    hmap::Array z;
    {
      if (this->wshape.x > (*p_input_hmap).shape.x ||
          this->wshape.y > (*p_input_hmap).shape.y)
        this->wshape = (*p_input_hmap).shape;

      hmap::Array z = (*p_input_hmap).to_array(this->wshape);

      this->value_out.dijkstra(z,
                               {0.f, 1.f, 0.f, 1.f},
                               0,
                               this->elevation_ratio,
                               this->distance_exponent);
    }
  }
}

void PathFinding::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
