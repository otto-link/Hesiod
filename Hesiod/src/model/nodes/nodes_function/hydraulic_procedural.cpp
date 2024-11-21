/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_procedural_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "ridge_mask", CONFIG);

  // attribute(s)
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<FloatAttribute>("ridge_wavelength",
                                   0.1f,
                                   0.01f,
                                   0.2f,
                                   "ridge_wavelength");
  p_node->add_attr<FloatAttribute>("ridge_scaling", 0.3f, 0.01f, 1.f, "ridge_scaling");
  p_node->add_attr<MapEnumAttribute>("erosion_profile",
                                     "Triangle smooth",
                                     erosion_profile_map,
                                     "erosion_profile");
  p_node->add_attr<FloatAttribute>("delta", 0.2f, 0.f, 1.f, "delta");
  p_node->add_attr<FloatAttribute>("noise_ratio", 0.2f, 0.f, 1.f, "noise_ratio");
  p_node->add_attr<FloatAttribute>("density_factor", 1.f, 0.1f, 10.f, "density_factor");
  p_node->add_attr<FloatAttribute>("kernel_width_ratio",
                                   2.f,
                                   0.1f,
                                   8.f,
                                   "kernel_width_ratio");
  p_node->add_attr<FloatAttribute>("phase_smoothing", 2.f, 0.f, 10.f, "phase_smoothing");
  p_node->add_attr<FloatAttribute>("phase_noise_amp", 3.f, 0.f, 6.f, "phase_noise_amp");
  p_node->add_attr<BoolAttribute>("reverse_phase", false, "reverse_phase");
  p_node->add_attr<BoolAttribute>("use_default_mask", true, "use_default_mask");
  p_node->add_attr<FloatAttribute>("slope_mask", 16.f, 0.f, 32.f, "slope_mask");

  // attribute(s) order
  p_node->set_attr_ordered_key({"seed",
                                "ridge_wavelength",
                                "ridge_scaling",
                                "erosion_profile",
                                "delta",
                                "noise_ratio",
                                "_SEPARATOR_",
                                "density_factor",
                                "kernel_width_ratio",
                                "phase_smoothing",
                                "phase_noise_amp",
                                "reverse_phase",
                                "_SEPARATOR_",
                                "use_default_mask",
                                "slope_mask"});
}

void compute_hydraulic_procedural_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_mask = p_node->get_value_ref<hmap::HeightMap>("mask");
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");
    hmap::HeightMap *p_ridge_mask = p_node->get_value_ref<hmap::HeightMap>("ridge_mask");

    // copy the input heightmap
    *p_out = *p_in;

    float hmin = p_in->min();
    float hmax = p_in->max();

    float talus_mask = GET("slope_mask", FloatAttribute) / (float)p_out->shape.x;
    float global_wavelength = GET("ridge_wavelength", FloatAttribute) *
                              (float)p_out->tiling.x;

    hmap::transform(*p_out,
                    p_mask,
                    p_ridge_mask,
                    [p_node, hmin, hmax, talus_mask, global_wavelength](
                        hmap::Array &h_out,
                        hmap::Array *p_mask_array,
                        hmap::Array *p_ridge_mask_array)
                    {
                      hmap::hydraulic_procedural(
                          h_out,
                          GET("seed", SeedAttribute),
                          global_wavelength,
                          GET("ridge_scaling", FloatAttribute),
                          (hmap::ErosionProfile)GET("erosion_profile", MapEnumAttribute),
                          GET("delta", FloatAttribute),
                          GET("noise_ratio", FloatAttribute),
                          -1, // prefilter_ir,
                          GET("density_factor", FloatAttribute),
                          GET("kernel_width_ratio", FloatAttribute),
                          GET("phase_smoothing", FloatAttribute),
                          GET("phase_noise_amp", FloatAttribute),
                          GET("reverse_phase", BoolAttribute),
                          GET("use_default_mask", BoolAttribute),
                          talus_mask,
                          p_mask_array,
                          p_ridge_mask_array,
                          hmin,
                          hmax);
                    });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
