/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

WhiteDensityMap::WhiteDensityMap(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("WhiteDensityMap::WhiteDensityMapFilter()");

  this->node_type = "WhiteDensityMap";
  this->category = category_mapping.at(this->node_type);
  this->add_port(
      gnode::Port("density map", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void WhiteDensityMap::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void WhiteDensityMap::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("density map"));

  this->value_out.set_sto(p_input_hmap->shape,
                          p_input_hmap->tiling,
                          p_input_hmap->overlap);

  hmap::transform(
      this->value_out,
      *p_input_hmap,
      [this](hmap::Array &h_out, hmap::Array &density_map)
      { h_out = hmap::white_density_map(density_map, (uint)this->seed); });
}

} // namespace hesiod::cnode
