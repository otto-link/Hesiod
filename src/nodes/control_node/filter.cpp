/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Filter::Filter(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("Filter::Filter()");
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("mask",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->add_port(gnode::Port("thru", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Filter::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  void *p_input_data = this->get_p_data("input");

  // can be used to pass a reference to the input to any downstrean
  // node
  this->set_p_data("thru", p_input_data);

  // standard output
  if (this->get_thru())
  {
    LOG_DEBUG("thru");
    this->set_p_data("output", p_input_data);
    this->update_links();
  }
  else
  {
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
}

void Filter::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));
  hmap::HeightMap *p_input_mask = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("mask"));

  if (this->get_thru())
  {
    // work directly on the input
    this->compute_filter(*p_input_hmap, p_input_mask);
  }
  else
  {
    // work on a copy of the input
    this->value_out = *p_input_hmap;
    this->compute_filter(this->value_out, p_input_mask);
  }
}

} // namespace hesiod::cnode
