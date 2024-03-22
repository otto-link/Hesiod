/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

HydraulicAlgebric::HydraulicAlgebric(std::string id)
    : ControlNode(id), Erosion(id)
{
  this->node_type = "HydraulicAlgebric";
  this->category = category_mapping.at(this->node_type);

  this->attr["talus_global"] = NEW_ATTR_FLOAT(2.f, 0.01f, 32.f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.01f, 0.01f, 0.2f);
  this->attr["c_erosion"] = NEW_ATTR_FLOAT(0.07f, 0.01f, 0.1f);
  this->attr["c_deposition"] = NEW_ATTR_FLOAT(0.01f, 0.01f, 0.1f);
  this->attr["iterations"] = NEW_ATTR_INT(1, 1, 10);

  this->attr_ordered_key = {"talus_global",
                            "radius",
                            "c_erosion",
                            "c_deposition",
                            "iterations"};
}

void HydraulicAlgebric::compute_erosion(hmap::HeightMap &h,
                                        hmap::HeightMap *p_bedrock,
                                        hmap::HeightMap *, // not used
                                        hmap::HeightMap *p_mask,
                                        hmap::HeightMap *p_erosion_map,
                                        hmap::HeightMap *p_deposition_map)
{
  LOG_DEBUG("computing erosion node [%s]", this->id.c_str());

  float talus = GET_ATTR_FLOAT("talus_global") / (float)this->value_out.shape.x;
  int   ir = std::max(1,
                    (int)(GET_ATTR_FLOAT("radius") * this->value_out.shape.x));

  hmap::transform(h,
                  p_bedrock,
                  nullptr,
                  p_mask,
                  p_erosion_map,
                  p_deposition_map,
                  [this, &talus, &ir](hmap::Array &h_out,
                                      hmap::Array *p_bedrock_array,
                                      hmap::Array *,
                                      hmap::Array *p_mask_array,
                                      hmap::Array *p_erosion_map_array,
                                      hmap::Array *p_deposition_map_array)
                  {
                    hmap::hydraulic_algebric(h_out,
                                             p_mask_array,
                                             talus,
                                             ir,
                                             p_bedrock_array,
                                             p_erosion_map_array,
                                             p_deposition_map_array,
                                             GET_ATTR_FLOAT("c_erosion"),
                                             GET_ATTR_FLOAT("c_deposition"),
                                             GET_ATTR_INT("iterations"));
                  });

  h.smooth_overlap_buffers();

  if (p_erosion_map)
    (*p_erosion_map).smooth_overlap_buffers();

  if (p_deposition_map)
    (*p_deposition_map).smooth_overlap_buffers();
}

} // namespace hesiod::cnode
