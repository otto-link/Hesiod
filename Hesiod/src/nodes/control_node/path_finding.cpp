/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

PathFinding::PathFinding(std::string id) : ControlNode(id)
{
  LOG_DEBUG("PathFinding::PathFinding()");
  this->node_type = "PathFinding";
  this->category = category_mapping.at(this->node_type);

  this->attr["shape"] = NEW_ATTR_SHAPE();
  this->attr["elevation_ratio"] = NEW_ATTR_FLOAT(0.5f, 0.f, 0.9f);
  this->attr["distance_exponent"] = NEW_ATTR_FLOAT(1.f, 0.5f, 1.f);

  this->attr_ordered_key = {};

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

  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "heightmap");
  hmap::HeightMap *p_mask = CAST_PORT_REF(hmap::HeightMap, "mask nogo");
  hmap::Path      *p_path = CAST_PORT_REF(hmap::Path, "path");

  // work on a copy of the input
  this->value_out = *p_path;

  GET_ATTR_REF_SHAPE("shape")->set_value_max(p_hmap->shape);
  hmap::Vec2<int> wshape = GET_ATTR_SHAPE("shape");

  if (p_path->get_npoints() > 1)
  // work on a subset of the data
  {
    if (wshape.x > (*p_hmap).shape.x || wshape.y > (*p_hmap).shape.y)
      wshape = (*p_hmap).shape;

    hmap::Array z = (*p_hmap).to_array(wshape);
    hmap::remap(z);

    hmap::Array *p_mask_array = nullptr;
    hmap::Array  mask_array;

    if (p_mask)
    {
      mask_array = p_mask->to_array(wshape);
      p_mask_array = &mask_array;
    }

    this->value_out.dijkstra(z,
                             {0.f, 1.f, 0.f, 1.f},
                             0,
                             GET_ATTR_FLOAT("elevation_ratio"),
                             GET_ATTR_FLOAT("distance_exponent"),
                             p_mask_array);
  }
}

void PathFinding::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
