/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_procedural_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "ridge_mask", CONFIG(node));

  // attribute(s)
  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<FloatAttribute>("ridge_wavelength",
                                "ridge_wavelength",
                                0.1f,
                                0.01f,
                                0.2f);
  node.add_attr<FloatAttribute>("ridge_scaling", "ridge_scaling", 0.3f, 0.01f, 1.f);
  node.add_attr<EnumAttribute>("erosion_profile",
                               "erosion_profile",
                               enum_mappings.erosion_profile_map,
                               "Triangle smooth");
  node.add_attr<FloatAttribute>("delta", "delta", 0.2f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("noise_ratio", "noise_ratio", 0.2f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("density_factor", "density_factor", 1.f, 0.1f, 10.f);
  node.add_attr<FloatAttribute>("kernel_width_ratio",
                                "kernel_width_ratio",
                                2.f,
                                0.1f,
                                8.f);
  node.add_attr<FloatAttribute>("phase_smoothing", "phase_smoothing", 2.f, 0.f, 10.f);
  node.add_attr<FloatAttribute>("phase_noise_amp", "phase_noise_amp", 0.f, 0.f, 6.f);
  node.add_attr<BoolAttribute>("reverse_phase", "reverse_phase", false);
  node.add_attr<BoolAttribute>("rotate90", "rotate90", false);
  node.add_attr<BoolAttribute>("use_default_mask", "use_default_mask", true);
  node.add_attr<FloatAttribute>("slope_mask", "slope_mask", 6.f, 0.f, 32.f);

  // attribute(s) order
  node.set_attr_ordered_key({"seed",
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

void compute_hydraulic_procedural_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_ridge_mask = node.get_value_ref<hmap::Heightmap>("ridge_mask");

    // copy the input heightmap
    *p_out = *p_in;

    float hmin = p_in->min();
    float hmax = p_in->max();

    float talus_mask = node.get_attr<FloatAttribute>("slope_mask") /
                       (float)p_out->shape.x;
    float global_wavelength = node.get_attr<FloatAttribute>("ridge_wavelength") *
                              (float)p_out->tiling.x;

    hmap::transform(
        {p_out, p_mask, p_ridge_mask},
        [&node, hmin, hmax, talus_mask, global_wavelength](
            std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];
          hmap::Array *pa_ridge_mask = p_arrays[2];

          hmap::hydraulic_procedural(
              *pa_out,
              node.get_attr<SeedAttribute>("seed"),
              global_wavelength,
              node.get_attr<FloatAttribute>("ridge_scaling"),
              (hmap::ErosionProfile)node.get_attr<EnumAttribute>("erosion_profile"),
              node.get_attr<FloatAttribute>("delta"),
              node.get_attr<FloatAttribute>("noise_ratio"),
              -1, // prefilter_ir,
              node.get_attr<FloatAttribute>("density_factor"),
              node.get_attr<FloatAttribute>("kernel_width_ratio"),
              node.get_attr<FloatAttribute>("phase_smoothing"),
              node.get_attr<FloatAttribute>("phase_noise_amp"),
              node.get_attr<BoolAttribute>("reverse_phase"),
              node.get_attr<BoolAttribute>("rotate90"),
              node.get_attr<BoolAttribute>("use_default_mask"),
              talus_mask,
              pa_mask,
              pa_ridge_mask,
              hmin,
              hmax);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();
  }
}

} // namespace hesiod
