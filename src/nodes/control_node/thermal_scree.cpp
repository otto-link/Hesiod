/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ThermalScree::ThermalScree(std::string id) : gnode::Node(id)
{
  this->node_type = "ThermalScree";
  this->category = category_mapping.at(this->node_type);

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("mask",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));

  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->add_port(
      gnode::Port("deposition map", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();

  this->deposition_map = hmap::HeightMap();
}

void ThermalScree::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
  this->set_p_data("deposition map", (void *)&(this->deposition_map));
}

void ThermalScree::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));
  hmap::HeightMap *p_input_mask = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("mask"));

  hmap::HeightMap *p_output_deposition_map = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("deposition map"));

  // work on a copy of the input
  this->value_out = *p_input_hmap;

  if (p_output_deposition_map)
    this->deposition_map.set_sto(p_input_hmap->shape,
                                 p_input_hmap->tiling,
                                 p_input_hmap->overlap);

  float talus = this->talus_global / (float)this->value_out.shape.x;

  hmap::transform(this->value_out,
                  p_input_mask,
                  p_output_deposition_map,
                  [this, &talus](hmap::Array &h_out,
                                 hmap::Array *p_mask_array,
                                 hmap::Array *p_deposition_map_array)
                  {
                    hmap::thermal_scree(h_out,
                                        p_mask_array,
                                        talus,
                                        (uint)this->seed,
                                        this->zmax,
                                        this->zmin,
                                        this->noise_ratio,
                                        p_deposition_map_array,
                                        this->landing_talus_ratio,
                                        this->landing_width_ratio,
                                        this->talus_constraint);
                  });

  this->value_out.smooth_overlap_buffers();

  if (p_output_deposition_map)
    p_output_deposition_map->smooth_overlap_buffers();
}

} // namespace hesiod::cnode
