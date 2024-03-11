/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

HydraulicStream::HydraulicStream(std::string id) : ControlNode(id), Erosion(id)
{
  this->node_type = "HydraulicStream";
  this->category = category_mapping.at(this->node_type);

  this->attr["c_erosion"] = NEW_ATTR_FLOAT(0.05f, 0.01f, 0.1f);
  this->attr["talus_ref"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 10.f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.f, 0.f, 0.1f);
  this->attr["clipping_ratio"] = NEW_ATTR_FLOAT(10.f, 0.1f, 100.f);

  this->attr_ordered_key = {"c_erosion",
                            "talus_ref",
                            "radius",
                            "clipping_ratio"};
}

void HydraulicStream::compute_erosion(hmap::HeightMap &h,
                                      hmap::HeightMap *p_bedrock,
                                      hmap::HeightMap *p_moisture_map,
                                      hmap::HeightMap *p_mask,
                                      hmap::HeightMap *p_erosion_map,
                                      hmap::HeightMap *p_deposition_map)
{
  LOG_DEBUG("computing erosion node [%s]", this->id.c_str());

  int ir = (int)(GET_ATTR_FLOAT("radius") * this->value_out.shape.x);

  hmap::transform(h,
                  p_bedrock,
                  p_moisture_map,
                  p_mask,
                  p_erosion_map,
                  p_deposition_map,
                  [this, &ir](hmap::Array &h_out,
                              hmap::Array *p_bedrock_array,
                              hmap::Array *p_moisture_map_array,
                              hmap::Array *p_mask_array,
                              hmap::Array *p_erosion_map_array,
                              hmap::Array *p_deposition_map_array)
                  {
                    hmap::hydraulic_stream(h_out,
                                           p_mask_array,
                                           GET_ATTR_FLOAT("c_erosion"),
                                           GET_ATTR_FLOAT("talus_ref"),
                                           p_bedrock_array,
                                           p_moisture_map_array,
                                           p_erosion_map_array,
                                           ir,
                                           GET_ATTR_FLOAT("clipping_ratio"));
                  });

  h.smooth_overlap_buffers();

  if (p_erosion_map)
    (*p_erosion_map).smooth_overlap_buffers();

  if (p_deposition_map)
    (*p_deposition_map).smooth_overlap_buffers();
}

} // namespace hesiod::cnode
