/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Warp::Warp(std::string id) : gnode::Node(id)
{
  this->node_type = "Warp";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("dx",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("dy",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));

  // this->add_port(gnode::Port("mask",
  //                            gnode::direction::in,
  //                            dtype::dHeightMap,
  //                            gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Warp::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());

  // retrieve infos if one the input is connected
  void *p_input_data = this->get_p_data("input");

  if (p_input_data != nullptr)
  {
    hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
        p_input_data);

    // reshape the storage based on the input heightmap, if not
    // already done
    if (this->shape != p_input_hmap->shape)
    {
      this->shape = p_input_hmap->shape;
      this->value_out.set_sto(p_input_hmap->shape,
                              p_input_hmap->tiling,
                              p_input_hmap->overlap);
      LOG_DEBUG("node [%s]: reshape inner storage to {%d, %d}",
                this->id.c_str(),
                this->shape.x,
                this->shape.y);
    }
  }
  else
    LOG_DEBUG("input not ready (connected or value set)");

  this->set_p_data("output", (void *)&(this->value_out));
}

void Warp::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  // work on a copy of the input
  this->value_out = *(hmap::HeightMap *)this->get_p_data("input");

  hmap::transform(this->value_out,
                  (hmap::HeightMap *)this->get_p_data("dx"),
                  (hmap::HeightMap *)this->get_p_data("dy"),
                  [this](hmap::Array &z, hmap::Array *p_dx, hmap::Array *p_dy)
                  { hmap::warp(z, p_dx, p_dy, this->scale); });
  this->value_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
