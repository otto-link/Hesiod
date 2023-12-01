/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

HydraulicVpipes::HydraulicVpipes(std::string id) : Erosion(id)
{
  this->node_type = "HydraulicVpipes";
  this->category = category_mapping.at(this->node_type);

  this->attr["iterations"] = NEW_ATTR_INT(50, 1, 500);
  this->attr["water_height"] = NEW_ATTR_FLOAT(0.01f, 0.01f, 0.5f);
  this->attr["c_capacity"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 0.5f);
  this->attr["c_erosion"] = NEW_ATTR_FLOAT(0.01f, 0.f, 0.5f);
  this->attr["c_deposition"] = NEW_ATTR_FLOAT(0.01f, 0.f, 0.5f);
  this->attr["rain_rate"] = NEW_ATTR_FLOAT(0.f, 0.f, 0.1f);
  this->attr["evap_rate"] = NEW_ATTR_FLOAT(0.01f, 0.01f, 0.1f);

  this->attr_ordered_key = {"iterations",
                            "water_height",
                            "c_capacity",
                            "c_erosion",
                            "c_deposition",
                            "rain_rate",
                            "evap_rate"};
}

void HydraulicVpipes::compute_erosion(hmap::HeightMap &h,
                                      hmap::HeightMap *p_bedrock,
                                      hmap::HeightMap *p_moisture_map,
                                      hmap::HeightMap *p_mask,
                                      hmap::HeightMap *p_erosion_map,
                                      hmap::HeightMap *p_deposition_map)
{
  LOG_DEBUG("computing erosion node [%s]", this->id.c_str());

  hmap::transform(h,
                  p_bedrock,
                  p_moisture_map,
                  p_mask,
                  p_erosion_map,
                  p_deposition_map,
                  [this](hmap::Array &h_out,
                         hmap::Array *p_bedrock_array,
                         hmap::Array *p_moisture_map_array,
                         hmap::Array *p_mask_array,
                         hmap::Array *p_erosion_map_array,
                         hmap::Array *p_deposition_map_array)
                  {
                    hydraulic_vpipes(h_out,
                                     p_mask_array,
                                     GET_ATTR_INT("iterations"),
                                     p_bedrock_array,
                                     p_moisture_map_array,
                                     p_erosion_map_array,
                                     p_deposition_map_array,
                                     GET_ATTR_FLOAT("water_height"),
                                     GET_ATTR_FLOAT("c_capacity"),
                                     GET_ATTR_FLOAT("c_erosion"),
                                     GET_ATTR_FLOAT("c_deposition"),
                                     GET_ATTR_FLOAT("rain_rate"),
                                     GET_ATTR_FLOAT("evap_rate"));
                  });

  h.smooth_overlap_buffers();

  if (p_erosion_map)
    (*p_erosion_map).smooth_overlap_buffers();

  if (p_deposition_map)
    (*p_deposition_map).smooth_overlap_buffers();
}

} // namespace hesiod::cnode
