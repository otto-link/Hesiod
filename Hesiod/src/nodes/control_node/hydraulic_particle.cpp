/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::cnode
{

HydraulicParticle::HydraulicParticle(std::string id)
    : ControlNode(id), Erosion(id)
{
  this->node_type = "HydraulicParticle";
  this->category = category_mapping.at(this->node_type);

  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["nparticles"] = NEW_ATTR_INT(40000, 1, 1000000);
  this->attr["c_capacity"] = NEW_ATTR_FLOAT(40.f, 0.1f, 100.f);
  this->attr["c_erosion"] = NEW_ATTR_FLOAT(0.05f, 0.01f, 0.1f);
  this->attr["c_deposition"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 0.1f);
  this->attr["drag_rate"] = NEW_ATTR_FLOAT(0.01f, 0.f, 1.f);
  this->attr["evap_rate"] = NEW_ATTR_FLOAT(0.001f, 0.f, 1.f);
  this->attr["c_radius"] = NEW_ATTR_INT(0, 0, 16);

  this->attr_ordered_key = {"seed",
                            "nparticles",
                            "c_capacity",
                            "c_erosion",
                            "c_deposition",
                            "drag_rate",
                            "evap_rate",
                            "c_radius"};
}

void HydraulicParticle::compute_erosion(hmap::HeightMap &h,
                                        hmap::HeightMap *p_bedrock,
                                        hmap::HeightMap *p_moisture_map,
                                        hmap::HeightMap *p_mask,
                                        hmap::HeightMap *p_erosion_map,
                                        hmap::HeightMap *p_deposition_map)
{
  LOG_DEBUG("computing erosion node [%s]", this->id.c_str());

  int nparticles_tile = (int)(GET_ATTR_INT("nparticles") /
                              (float)h.get_ntiles());

  hmap::transform(h,
                  p_bedrock,
                  p_moisture_map,
                  p_mask,
                  p_erosion_map,
                  p_deposition_map,
                  [this, &nparticles_tile](hmap::Array &h_out,
                                           hmap::Array *p_bedrock_array,
                                           hmap::Array *p_moisture_map_array,
                                           hmap::Array *p_mask_array,
                                           hmap::Array *p_erosion_map_array,
                                           hmap::Array *p_deposition_map_array)
                  {
                    hmap::hydraulic_particle(h_out,
                                             p_mask_array,
                                             nparticles_tile,
                                             GET_ATTR_SEED("seed"),
                                             p_bedrock_array,
                                             p_moisture_map_array,
                                             p_erosion_map_array,
                                             p_deposition_map_array,
                                             GET_ATTR_INT("c_radius"),
                                             GET_ATTR_FLOAT("c_capacity"),
                                             GET_ATTR_FLOAT("c_erosion"),
                                             GET_ATTR_FLOAT("c_deposition"),
                                             GET_ATTR_FLOAT("drag_rate"),
                                             GET_ATTR_FLOAT("evap_rate"));
                  });
}

} // namespace hesiod::cnode
