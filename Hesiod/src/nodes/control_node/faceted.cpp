/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Faceted::Faceted(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Faceted::Faceted()");
  this->node_type = "Faceted";
  this->category = category_mapping.at(this->node_type);

  this->attr["neighborhood"] = NEW_ATTR_MAPENUM(this->neighborhood_map);

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  // this->add_port(gnode::Port("dx",
  //                            gnode::direction::in,
  //                            dtype::dHeightMap,
  //                            gnode::optional::yes));
  // this->add_port(gnode::Port("dy",
  //                            gnode::direction::in,
  //                            dtype::dHeightMap,
  //                            gnode::optional::yes));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Faceted::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void Faceted::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  // hmap::HeightMap *p_dx = CAST_PORT_REF(hmap::HeightMap, "dx");
  // hmap::HeightMap *p_dy = CAST_PORT_REF(hmap::HeightMap, "dy");

  this->value_out = *p_input_hmap;

  {
    // hmap::transform(
    //     this->value_out, // output
    //     p_dx,
    //     p_dy,
    //     [this](hmap::Array &h, hmap::Array *p_noise_x, hmap::Array
    //     *p_noise_y)
    //     {
    //       h = hmap::faceted(h,
    //                         GET_ATTR_MAPENUM("neighborhood"),
    //                         p_noise_x,
    //                         p_noise_y);
    //     });
  }

  // --- work on a single array as a temporary solution
  hmap::Array z_array = this->value_out.to_array();
  z_array = hmap::faceted(z_array, GET_ATTR_MAPENUM("neighborhood"));
  this->value_out.from_array_interp(z_array);
}

} // namespace hesiod::cnode
