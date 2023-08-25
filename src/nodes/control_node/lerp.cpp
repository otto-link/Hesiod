/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Lerp::Lerp(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("Lerp::Lerp()");
  this->node_type = "Lerp";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("a", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("b", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("t",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Lerp::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());

  // retrieve infos if one the input is connected
  void *p_input_data = this->get_p_data("a") == nullptr
                           ? (this->get_p_data("b") == nullptr
                                  ? nullptr
                                  : this->get_p_data("b"))
                           : this->get_p_data("a");

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

void Lerp::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap_a = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("a"));
  hmap::HeightMap *p_input_hmap_b = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("b"));
  hmap::HeightMap *p_input_hmap_t = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("t"));

  if (p_input_hmap_t)
    hmap::transform(
        this->value_out, // output
        *p_input_hmap_a, // input
        *p_input_hmap_b, // input
        *p_input_hmap_t, // input
        [](hmap::Array &out, hmap::Array &a, hmap::Array &b, hmap::Array &t)
        { out = hmap::lerp(a, b, t); });
  else
    hmap::transform(this->value_out, // output
                    *p_input_hmap_a, // input
                    *p_input_hmap_b, // input
                    [this](hmap::Array &out, hmap::Array &a, hmap::Array &b)
                    { out = hmap::lerp(a, b, this->t); });
}

} // namespace hesiod::cnode
