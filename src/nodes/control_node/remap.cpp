/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Remap::Remap(std::string id) : Unary(id)
{
  this->category += "/Range";
  this->node_type = "Remap";
  this->update_inner_bindings();
}

float Remap::get_vmin()
{
  return this->vmin;
}

float Remap::get_vmax()
{
  return this->vmax;
}

void Remap::set_vmin(float new_vmin)
{
  if (new_vmin != this->vmin)
  {
    this->vmin = new_vmin;
    this->force_update();
  }
}

void Remap::set_vmax(float new_vmax)
{
  if (new_vmax != this->vmax)
  {
    this->vmax = new_vmax;
    this->force_update();
  }
}

void Remap::compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  // make a copy of the input and applied range remapping
  h_out = *p_h_in;
  h_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
