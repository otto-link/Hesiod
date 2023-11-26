/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

PathToHeightmap::PathToHeightmap(std::string     id,
                                 hmap::Vec2<int> shape,
                                 hmap::Vec2<int> tiling,
                                 float           overlap)
    : ControlNode(id), shape(shape), tiling(tiling), overlap(overlap)
{
  LOG_DEBUG("PathToHeightmap::PathToHeightmap()");
  this->node_type = "PathToHeightmap";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("path", gnode::direction::in, dtype::dPath));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void PathToHeightmap::compute()
{
  LOG_DEBUG("computing PathToHeightmap node [%s]", this->id.c_str());

  hmap::Path *p_input_path = static_cast<hmap::Path *>(
      (void *)this->get_p_data("path"));

  if (p_input_path->get_npoints() > 1)
  {
    if (!this->filled)
    {
      hmap::fill(this->value_out,
                 [p_input_path](hmap::Vec2<int>   shape,
                                hmap::Vec2<float> shift,
                                hmap::Vec2<float> scale)
                 {
                   hmap::Array       z = hmap::Array(shape);
                   hmap::Vec4<float> bbox = hmap::Vec4<float>(shift.x,
                                                              shift.x + scale.x,
                                                              shift.y,
                                                              shift.y +
                                                                  scale.y);
                   p_input_path->to_array(z, bbox);
                   return z;
                 });
    }
    else
    {
      // work on a single array as a temporary solution
      hmap::Array       z_array = hmap::Array(this->value_out.shape);
      hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);

      p_input_path->to_array(z_array, bbox, true);
      this->value_out.from_array_interp(z_array);
    }
    this->value_out.remap(this->vmin, this->vmax);
  }
  else
    // fill with zeros
    hmap::transform(this->value_out, [](hmap::Array x) { x = 0.f; });
}

void PathToHeightmap::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

} // namespace hesiod::cnode
