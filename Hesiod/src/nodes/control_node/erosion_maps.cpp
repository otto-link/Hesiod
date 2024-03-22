/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ErosionMaps::ErosionMaps(std::string id) : ControlNode(id)
{
  this->node_type = "ErosionMaps";
  this->category = category_mapping.at(this->node_type);

  this->attr["tolerance"] = NEW_ATTR_FLOAT(0.f, 0.f, 0.5f);

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
  hmap::HeightMap *p_before = CAST_PORT_REF(hmap::HeightMap, "1 initial");
  hmap::HeightMap *p_after = CAST_PORT_REF(hmap::HeightMap, "2 eroded");

  // work on a copy of the input
  this->erosion_map.set_sto(p_before->shape,
                            p_before->tiling,
                            p_before->overlap);

  this->deposition_map.set_sto(p_before->shape,
                               p_before->tiling,
                               p_before->overlap);

  hmap::transform(*p_before,
                  *p_after,
                  this->erosion_map,
                  this->deposition_map,
                  [this](hmap::Array &z_before,
                         hmap::Array &z_after,
                         hmap::Array &emap,
                         hmap::Array &dmap)
                  {
                    hmap::erosion_maps(z_before,
                                       z_after,
                                       emap,
                                       dmap,
                                       GET_ATTR_FLOAT("tolerance"));
                  });
}

} // namespace hesiod::cnode
