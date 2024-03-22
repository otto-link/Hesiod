/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ThermalScree::ThermalScree(std::string id) : ControlNode(id)
{
  this->node_type = "ThermalScree";
  this->category = category_mapping.at(this->node_type);

  this->attr["talus_constraint"] = NEW_ATTR_BOOL(true);
  this->attr["talus_global"] = NEW_ATTR_FLOAT(3.f, 1.f, 8.f);
  this->attr["zmin"] = NEW_ATTR_FLOAT(-1.f, -1.f, 2.f);
  this->attr["zmax"] = NEW_ATTR_FLOAT(0.3f, -1.f, 2.f);
  this->attr["noise_ratio"] = NEW_ATTR_FLOAT(0.3f, 0.f, 1.f);
  this->attr["landing_talus_ratio"] = NEW_ATTR_FLOAT(1.f, 0.f, 1.f);
  this->attr["landing_width_ratio"] = NEW_ATTR_FLOAT(0.25f, 0.01f, 1.f);
  this->attr["seed"] = NEW_ATTR_SEED();

  this->attr_ordered_key = {"talus_constraint",
                            "talus_global",
                            "zmin",
                            "zmax",
                            "noise_ratio",
                            "landing_talus_ratio",
                            "landing_width_ratio",
                            "seed"};

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
  hmap::HeightMap *p_hmap = CAST_PORT_REF(hmap::HeightMap, "input");
  hmap::HeightMap *p_mask = CAST_PORT_REF(hmap::HeightMap, "mask");

  hmap::HeightMap *p_deposition_map = CAST_PORT_REF(hmap::HeightMap,
                                                    "deposition map");

  // work on a copy of the input
  this->value_out = *p_hmap;

  if (p_deposition_map)
    this->deposition_map.set_sto(p_hmap->shape,
                                 p_hmap->tiling,
                                 p_hmap->overlap);

  float talus = GET_ATTR_FLOAT("talus_global") / (float)this->value_out.shape.x;

  hmap::transform(this->value_out,
                  p_mask,
                  p_deposition_map,
                  [this, &talus](hmap::Array &h_out,
                                 hmap::Array *p_mask_array,
                                 hmap::Array *p_deposition_map_array)
                  {
                    hmap::thermal_scree(h_out,
                                        p_mask_array,
                                        talus,
                                        GET_ATTR_SEED("seed"),
                                        GET_ATTR_FLOAT("zmax"),
                                        GET_ATTR_FLOAT("zmin"),
                                        GET_ATTR_FLOAT("noise_ratio"),
                                        p_deposition_map_array,
                                        GET_ATTR_FLOAT("landing_talus_ratio"),
                                        GET_ATTR_FLOAT("landing_width_ratio"),
                                        GET_ATTR_BOOL("talus_constraint"));
                  });

  this->value_out.smooth_overlap_buffers();

  if (p_deposition_map)
    p_deposition_map->smooth_overlap_buffers();
}

} // namespace hesiod::cnode
