/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_procedural_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "ridge_mask", CONFIG);

  // attribute(s)
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(FloatAttribute, "ridge_wavelength", 0.1f, 0.01f, 0.2f);
  ADD_ATTR(FloatAttribute, "ridge_scaling", 0.3f, 0.01f, 1.f);
  ADD_ATTR(EnumAttribute,
           "erosion_profile",
           HSD_CTX.enum_mappings.erosion_profile_map,
           "Triangle smooth");
  ADD_ATTR(FloatAttribute, "delta", 0.2f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "noise_ratio", 0.2f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "density_factor", 1.f, 0.1f, 10.f);
  ADD_ATTR(FloatAttribute, "kernel_width_ratio", 2.f, 0.1f, 8.f);
  ADD_ATTR(FloatAttribute, "phase_smoothing", 2.f, 0.f, 10.f);
  ADD_ATTR(FloatAttribute, "phase_noise_amp", 0.f, 0.f, 6.f);
  ADD_ATTR(BoolAttribute, "reverse_phase", false);
  ADD_ATTR(BoolAttribute, "rotate90", false);
  ADD_ATTR(BoolAttribute, "use_default_mask", true);
  ADD_ATTR(FloatAttribute, "slope_mask", 6.f, 0.f, 32.f);

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
                                "rotate90",
                                "_SEPARATOR_",
                                "use_default_mask",
                                "slope_mask"});
}

void compute_hydraulic_procedural_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_ridge_mask = p_node->get_value_ref<hmap::Heightmap>("ridge_mask");

    // copy the input heightmap
    *p_out = *p_in;

    float hmin = p_in->min();
    float hmax = p_in->max();

    float talus_mask = GET("slope_mask", FloatAttribute) / (float)p_out->shape.x;
    float global_wavelength = GET("ridge_wavelength", FloatAttribute) *
                              (float)p_out->tiling.x;

    hmap::transform(
        {p_out, p_mask, p_ridge_mask},
        [p_node, hmin, hmax, talus_mask, global_wavelength](
            std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];
          hmap::Array *pa_ridge_mask = p_arrays[2];

          hmap::hydraulic_procedural(
              *pa_out,
              GET("seed", SeedAttribute),
              global_wavelength,
              GET("ridge_scaling", FloatAttribute),
              (hmap::ErosionProfile)GET("erosion_profile", EnumAttribute),
              GET("delta", FloatAttribute),
              GET("noise_ratio", FloatAttribute),
              -1, // prefilter_ir,
              GET("density_factor", FloatAttribute),
              GET("kernel_width_ratio", FloatAttribute),
              GET("phase_smoothing", FloatAttribute),
              GET("phase_noise_amp", FloatAttribute),
              GET("reverse_phase", BoolAttribute),
              GET("rotate90", BoolAttribute),
              GET("use_default_mask", BoolAttribute),
              talus_mask,
              pa_mask,
              pa_ridge_mask,
              hmin,
              hmax);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
