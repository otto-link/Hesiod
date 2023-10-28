/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

HydraulicRidge::HydraulicRidge(std::string id) : Filter(id)
{
  this->node_type = "HydraulicRidge";
  this->category = category_mapping.at(this->node_type);
}

void HydraulicRidge::compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  {
    // TODO - reactivate distributed calculation when the core procedure
    // will be compatible with tiling

    // hmap::transform(h,
    //                 p_mask,
    //                 [this](hmap::Array &x, hmap::Array *p_mask)
    //                 {
    //                   hmap::hydraulic_ridge(x,
    //                                         this->talus,
    //                                         p_mask,
    //                                         this->intensity,
    //                                         this->erosion_factor,
    //                                         this->smoothing_factor,
    //                                         this->noise_ratio,
    //                                         this->ir,
    //                                         (uint)this->seed);
    //                 });
    // h.smooth_overlap_buffers();
  }

  // --- work on a single array as a temporary solution
  hmap::Array z_array = h.to_array();

  // handle masking
  hmap::Array *p_mask_array = nullptr;
  hmap::Array  mask_array;

  if (p_mask)
  {
    mask_array = p_mask->to_array();
    p_mask_array = &mask_array;
  }

  // backup amplitude for post-process remapping
  float zmin = z_array.min();
  float zmax = z_array.max();

  float talus = this->talus_global / (float)this->value_out.shape.x;

  hmap::hydraulic_ridge(z_array,
                        talus,
                        p_mask_array,
                        this->intensity,
                        this->erosion_factor,
                        this->smoothing_factor,
                        this->noise_ratio,
                        this->ir,
                        (uint)this->seed);

  hmap::remap(z_array, zmin, zmax);

  h.from_array_interp(z_array);
}

} // namespace hesiod::cnode
