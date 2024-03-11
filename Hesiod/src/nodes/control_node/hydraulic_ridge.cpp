/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

HydraulicRidge::HydraulicRidge(std::string id) : ControlNode(id), Filter(id)
{
  this->node_type = "HydraulicRidge";
  this->category = category_mapping.at(this->node_type);

  this->attr["talus_global"] = NEW_ATTR_FLOAT(16.f, 0.01f, 32.f);
  this->attr["intensity"] = NEW_ATTR_FLOAT(0.5f, 0.f, 2.f);
  this->attr["erosion_factor"] = NEW_ATTR_FLOAT(1.5f, 0.f, 4.f);
  this->attr["smoothing_factor"] = NEW_ATTR_FLOAT(0.5f, 0.01f, 2.f);
  this->attr["noise_ratio"] = NEW_ATTR_FLOAT(0.1f, 0.f, 1.f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.01f, 0.f, 0.1f);
  this->attr["seed"] = NEW_ATTR_SEED();

  this->attr_ordered_key = {"talus_global",
                            "intensity",
                            "erosion_factor",
                            "smoothing_factor",
                            "noise_ratio",
                            "radius",
                            "seed"};
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

  float talus = GET_ATTR_FLOAT("talus_global") / (float)this->value_out.shape.x;
  int   ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * h.shape.x));

  hmap::hydraulic_ridge(z_array,
                        talus,
                        p_mask_array,
                        GET_ATTR_FLOAT("intensity"),
                        GET_ATTR_FLOAT("erosion_factor"),
                        GET_ATTR_FLOAT("smoothing_factor"),
                        GET_ATTR_FLOAT("noise_ratio"),
                        ir,
                        GET_ATTR_SEED("seed"));

  hmap::remap(z_array, zmin, zmax);

  h.from_array_interp(z_array);
}

} // namespace hesiod::cnode
