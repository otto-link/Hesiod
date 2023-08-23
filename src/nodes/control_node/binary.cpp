/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Binary::Binary(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("Binary::Binary()");
  this->add_port(
      gnode::Port("input##1", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("input##2", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
}

void Binary::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());

  // retrieve infos if one the input is connected
  void *p_input_data = this->get_p_data("input##1") == nullptr
                           ? (this->get_p_data("input##2") == nullptr
                                  ? nullptr
                                  : this->get_p_data("input##2"))
                           : this->get_p_data("input##1");

  if (p_input_data != nullptr)
  {
    hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
        p_input_data);

    // reshape the storage based on the input heightmap, if not
    // already done
    if (this->shape != p_input_hmap->shape)
    {
      this->shape = p_input_hmap->shape;
      this->value_out.set_shape(this->shape);
      this->value_out.set_tiling(p_input_hmap->tiling);
      this->value_out.set_overlap(p_input_hmap->overlap);
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

void Binary::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap1 = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input##1"));
  hmap::HeightMap *p_input_hmap2 = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input##2"));

  this->compute_in_out(this->value_out, p_input_hmap1, p_input_hmap2);
}

} // namespace hesiod::cnode
