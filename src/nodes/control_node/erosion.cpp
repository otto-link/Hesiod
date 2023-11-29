/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Erosion::Erosion(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Erosion::Erosion()");
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("bedrock",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("moisture map",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("mask",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));

  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->add_port(
      gnode::Port("erosion map", gnode::direction::out, dtype::dHeightMap));
  this->add_port(
      gnode::Port("deposition map", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();

  this->erosion_map = hmap::HeightMap();
  this->deposition_map = hmap::HeightMap();
}

void Erosion::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
  this->set_p_data("deposition map", (void *)&(this->deposition_map));
  this->set_p_data("erosion map", (void *)&(this->erosion_map));
}

void Erosion::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_bedrock = CAST_PORT_REF(hmap::HeightMap, "bedrock");
  hmap::HeightMap *p_moisture = CAST_PORT_REF(hmap::HeightMap, "moisture map");
  hmap::HeightMap *p_mask = CAST_PORT_REF(hmap::HeightMap, "mask");

  hmap::HeightMap *p_erosion = CAST_PORT_REF(hmap::HeightMap, "erosion map");
  hmap::HeightMap *p_deposition = CAST_PORT_REF(hmap::HeightMap,
                                                "deposition map");

  // work on a copy of the input
  this->value_out = *p_hmap;

  if (p_erosion)
    this->erosion_map.set_sto(p_hmap->shape, p_hmap->tiling, p_hmap->overlap);

  if (p_deposition)
    this->deposition_map.set_sto(p_hmap->shape,
                                 p_hmap->tiling,
                                 p_hmap->overlap);

  // if there is a moisture map, work on a copy to be able to
  // normalize it
  hmap::HeightMap *p_moisture_copy = nullptr;
  hmap::HeightMap  moisture_map_copy;
  if (p_moisture)
  {
    moisture_map_copy = (*p_moisture);
    moisture_map_copy.remap();
    p_moisture_copy = &moisture_map_copy;
  }

  this->compute_erosion(this->value_out,
                        p_bedrock,
                        p_moisture_copy,
                        p_mask,
                        p_erosion,
                        p_deposition);

  this->value_out.smooth_overlap_buffers();

  if (p_erosion)
    (*p_erosion).smooth_overlap_buffers();

  if (p_deposition)
    (*p_deposition).smooth_overlap_buffers();
}

} // namespace hesiod::cnode
