/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

HydraulicAlgebric::HydraulicAlgebric(std::string id) : Erosion(id)
{
  this->node_type = "HydraulicAlgebric";
  this->category = category_mapping.at(this->node_type);
}

void HydraulicAlgebric::compute_erosion(hmap::HeightMap &h,
                                        hmap::HeightMap *p_bedrock,
                                        hmap::HeightMap *, // not used
                                        hmap::HeightMap *p_mask,
                                        hmap::HeightMap *p_erosion_map,
                                        hmap::HeightMap *p_deposition_map)
{
  LOG_DEBUG("computing erosion node [%s]", this->id.c_str());

  float talus = this->talus_global / (float)this->value_out.shape.x;

  hmap::transform(h,
                  p_bedrock,
                  nullptr,
                  p_mask,
                  p_erosion_map,
                  p_deposition_map,
                  [this, &talus](hmap::Array &h_out,
                                 hmap::Array *p_bedrock_array,
                                 hmap::Array *,
                                 hmap::Array *p_mask_array,
                                 hmap::Array *p_erosion_map_array,
                                 hmap::Array *p_deposition_map_array)
                  {
                    hmap::hydraulic_algebric(h_out,
                                             p_mask_array,
                                             talus,
                                             this->ir,
                                             p_bedrock_array,
                                             p_erosion_map_array,
                                             p_deposition_map_array,
                                             this->c_erosion,
                                             this->c_deposition,
                                             this->iterations);
                  });

  h.smooth_overlap_buffers();

  if (p_erosion_map)
    (*p_erosion_map).smooth_overlap_buffers();

  if (p_deposition_map)
    (*p_deposition_map).smooth_overlap_buffers();
}

} // namespace hesiod::cnode
