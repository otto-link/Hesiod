/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

HydraulicStreamLog::HydraulicStreamLog(std::string id)
    : ControlNode(id), Erosion(id)
{
  this->node_type = "HydraulicStreamLog";
  this->category = category_mapping.at(this->node_type);

  this->attr["c_erosion"] = NEW_ATTR_FLOAT(0.05f, 0.01f, 0.1f);
  this->attr["talus_ref"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 10.f);
  this->attr["ir"] = NEW_ATTR_INT(0, 1, 16);
  this->attr["gamma"] = NEW_ATTR_FLOAT(1.f, 0.01f, 4.f);

  this->attr_ordered_key = {"c_erosion", "talus_ref", "ir", "gamma"};
}

void HydraulicStreamLog::compute_erosion(hmap::HeightMap &h,
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
                    hmap::hydraulic_stream_log(h_out,
                                               p_mask_array,
                                               GET_ATTR_FLOAT("c_erosion"),
                                               GET_ATTR_FLOAT("talus_ref"),
                                               GET_ATTR_FLOAT("gamma"),
                                               p_bedrock_array,
                                               p_moisture_map_array,
                                               p_erosion_map_array,
                                               GET_ATTR_INT("ir"));
                  });

  h.smooth_overlap_buffers();

  if (p_erosion_map)
    (*p_erosion_map).smooth_overlap_buffers();

  if (p_deposition_map)
    (*p_deposition_map).smooth_overlap_buffers();
}

} // namespace hesiod::cnode
