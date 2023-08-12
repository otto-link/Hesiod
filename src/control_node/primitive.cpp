/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Primitive::Primitive(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap)
    : gnode::Node(id), shape(shape), tiling(tiling), overlap(overlap)
{
  LOG_DEBUG("Primitive::Primitive()");
  this->add_port(gnode::Port("dx",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("dy",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

hmap::Vec2<int> Primitive::get_shape()
{
  return this->shape;
}

float Primitive::get_vmin()
{
  return this->vmin;
}

float Primitive::get_vmax()
{
  return this->vmax;
}

void Primitive::set_shape(hmap::Vec2<int> new_shape)
{
  if (new_shape != this->shape)
  {
    this->shape = new_shape;
    this->force_update();
  }
}

void Primitive::set_tiling(hmap::Vec2<int> new_tiling)
{
  if (new_tiling != this->tiling)
  {
    this->tiling = new_tiling;
    this->force_update();
  }
}

void Primitive::set_vmin(float new_vmin)
{
  if (new_vmin != this->vmin)
  {
    this->vmin = new_vmin;
    this->force_update();
  }
}

void Primitive::set_vmax(float new_vmax)
{
  if (new_vmax != this->vmax)
  {
    this->vmax = new_vmax;
    this->force_update();
  }
}

void Primitive::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
