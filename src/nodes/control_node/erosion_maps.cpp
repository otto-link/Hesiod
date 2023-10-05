/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ErosionMaps::ErosionMaps(std::string id) : gnode::Node(id)
{
  this->node_type = "ErosionMaps";
  this->category = category_mapping.at(this->node_type);

  this->add_port(
      gnode::Port("1 initial", gnode::direction::in, dtype::dHeightMap));

  this->add_port(
      gnode::Port("2 eroded", gnode::direction::in, dtype::dHeightMap));

  this->add_port(
      gnode::Port("erosion_map", gnode::direction::out, dtype::dHeightMap));
  this->add_port(
      gnode::Port("deposition_map", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void ErosionMaps::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("erosion_map", (void *)&(this->erosion_map));
  this->set_p_data("deposition_map", (void *)&(this->deposition_map));
}

void ErosionMaps::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_before = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("1 initial"));
  hmap::HeightMap *p_input_after = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("2 eroded"));

  // work on a copy of the input
  this->erosion_map.set_sto(p_input_before->shape,
                            p_input_before->tiling,
                            p_input_before->overlap);

  this->deposition_map.set_sto(p_input_before->shape,
                               p_input_before->tiling,
                               p_input_before->overlap);

  hmap::transform(
      *p_input_before,
      *p_input_after,
      this->erosion_map,
      this->deposition_map,
      [this](hmap::Array &z_before,
             hmap::Array &z_after,
             hmap::Array &emap,
             hmap::Array &dmap)
      { hmap::erosion_maps(z_before, z_after, emap, dmap, this->tolerance); });
}

} // namespace hesiod::cnode
