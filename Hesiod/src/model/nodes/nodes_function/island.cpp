/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/heightmap.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_island_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "land_mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "inland_mask", CONFIG(node));

  // attribute(s)
  std::vector<float> kw = {4.f, 4.f};

  node.add_attr<SeedAttribute>("seed", "seed");
  node.add_attr<FloatAttribute>("noise_amp", "noise_amp", 0.07f, 0.f, 2.f);
  node.add_attr<WaveNbAttribute>("noise_kw", "noise_kw", kw, 0.f, FLT_MAX, true);
  node.add_attr<IntAttribute>("noise_octaves", "noise_octaves", 8, INT_MAX);
  node.add_attr<FloatAttribute>("noise_rugosity", "noise_rugosity", 0.7f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("noise_angle", "noise_angle", 45.f, -180.f, 180.f);
  node.add_attr<FloatAttribute>("noise_k_smoothing",
                                "noise_k_smoothing",
                                0.05f,
                                0.f,
                                1.f);
  node.add_attr<FloatAttribute>("elevation_scale", "elevation_scale", 0.4f, 0.f, 2.f);
  node.add_attr<BoolAttribute>("filter_distance", "filter_distance", true);
  node.add_attr<FloatAttribute>("filter_radius", "filter_radius", 0.01f, 0.f, 0.2f);
  node.add_attr<FloatAttribute>("slope_min", "slope_min", 0.05f, 0.f, 32.f);
  node.add_attr<FloatAttribute>("slope_max", "slope_max", 1.5f, 0.f, 32.f);
  node.add_attr<FloatAttribute>("slope_start", "slope_start", 0.5f, 0.f, 4.f);
  node.add_attr<FloatAttribute>("slope_end", "slope_end", 1.5f, 0.f, 4.f);
  node.add_attr<FloatAttribute>("slope_noise_intensity",
                                "slope_noise_intensity",
                                0.f,
                                0.f,
                                100.f);
  node.add_attr<FloatAttribute>("k_smooth", "k_smooth", 0.05f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("radial_noise_intensity",
                                "radial_noise_intensity",
                                0.5f,
                                0.f,
                                2.f);
  node.add_attr<FloatAttribute>("radial_profile_gain",
                                "radial_profile_gain",
                                1.f,
                                0.01f,
                                10.f);
  node.add_attr<FloatAttribute>("water_decay", "water_decay", 0.1f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("water_depth", "water_depth", 0.3f, 0.f, FLT_MAX);
  node.add_attr<FloatAttribute>("lee_angle", "lee_angle", 30.f, -180.f, 180.f);
  node.add_attr<FloatAttribute>("lee_amp", "lee_amp", 0.f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("uplift_amp", "uplift_amp", 0.f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"seed",
                             "noise_amp",
                             "noise_kw",
                             "noise_octaves",
                             "noise_rugosity",
                             "noise_angle",
                             "noise_k_smoothing",
                             "elevation_scale",
                             "filter_distance",
                             "filter_radius",
                             "slope_min",
                             "slope_max",
                             "slope_start",
                             "slope_end",
                             "slope_noise_intensity",
                             "k_smooth",
                             "radial_noise_intensity",
                             "radial_profile_gain",
                             "water_decay",
                             "water_depth",
                             "lee_angle",
                             "lee_amp",
                             "uplift_amp"});

  setup_post_process_heightmap_attributes(node);

  // disable post-processing remap by default
  node.get_attr_ref<RangeAttribute>("post_remap")->set_is_active(false);
  node.get_attr_ref<RangeAttribute>("post_remap")->save_initial_state();
}

void compute_island_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_land = node.get_value_ref<hmap::Heightmap>("land_mask");
  if (!p_land)
    return;

  hmap::Heightmap *p_dr = node.get_value_ref<hmap::Heightmap>("dr");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("out");
  hmap::Heightmap *p_depth = node.get_value_ref<hmap::Heightmap>("water_depth");
  hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("inland_mask");

  int ir = std::max(
      1,
      (int)(node.get_attr<FloatAttribute>("filter_radius") * p_out->shape.x));

  float scale = node.get_attr<FloatAttribute>("elevation_scale");

  hmap::transform(
      {p_out, p_land, p_dr, p_depth, p_mask},
      [&node, ir, scale](std::vector<hmap::Array *> p_arrays)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_land = p_arrays[1];
        hmap::Array *pa_dr = p_arrays[2];
        hmap::Array *pa_depth = p_arrays[3];
        hmap::Array *pa_mask = p_arrays[4];

        // TODO replace apex elevation by global scale (centered on zero)

        if (pa_dr)
        {
          *pa_out = hmap::gpu::island(
              *pa_land,
              pa_dr,
              scale,
              node.get_attr<BoolAttribute>("filter_distance"),
              ir,
              scale * node.get_attr<FloatAttribute>("slope_min"),
              scale * node.get_attr<FloatAttribute>("slope_max"),
              scale * node.get_attr<FloatAttribute>("slope_start"),
              scale * node.get_attr<FloatAttribute>("slope_end"),
              node.get_attr<FloatAttribute>("slope_noise_intensity"),
              node.get_attr<FloatAttribute>("k_smooth"),
              node.get_attr<FloatAttribute>("radial_noise_intensity"),
              node.get_attr<FloatAttribute>("radial_profile_gain"),
              node.get_attr<FloatAttribute>("water_decay"),
              scale * node.get_attr<FloatAttribute>("water_depth"),
              node.get_attr<FloatAttribute>("lee_angle"),
              scale * node.get_attr<FloatAttribute>("lee_amp"),
              scale * node.get_attr<FloatAttribute>("uplift_amp"),
              pa_depth,
              pa_mask);
        }
        else
        {
          *pa_out = hmap::gpu::island(
              *pa_land,
              node.get_attr<SeedAttribute>("seed"),
              node.get_attr<FloatAttribute>("noise_amp"),
              node.get_attr<WaveNbAttribute>("noise_kw"),
              node.get_attr<IntAttribute>("noise_octaves"),
              node.get_attr<FloatAttribute>("noise_rugosity"),
              node.get_attr<FloatAttribute>("noise_angle"),
              node.get_attr<FloatAttribute>("noise_k_smoothing"),
              scale,
              node.get_attr<BoolAttribute>("filter_distance"),
              ir,
              scale * node.get_attr<FloatAttribute>("slope_min"),
              scale * node.get_attr<FloatAttribute>("slope_max"),
              scale * node.get_attr<FloatAttribute>("slope_start"),
              scale * node.get_attr<FloatAttribute>("slope_end"),
              node.get_attr<FloatAttribute>("slope_noise_intensity"),
              node.get_attr<FloatAttribute>("k_smooth"),
              node.get_attr<FloatAttribute>("radial_noise_intensity"),
              node.get_attr<FloatAttribute>("radial_profile_gain"),
              node.get_attr<FloatAttribute>("water_decay"),
              scale * node.get_attr<FloatAttribute>("water_depth"),
              node.get_attr<FloatAttribute>("lee_angle"),
              scale * node.get_attr<FloatAttribute>("lee_amp"),
              scale * node.get_attr<FloatAttribute>("uplift_amp"),
              pa_depth,
              pa_mask);
        }

        pa_depth->infos("depth");
      },
      node.get_config_ref()->hmap_transform_mode_gpu);

  // post-process
  p_out->smooth_overlap_buffers();
  // p_depth->smooth_overlap_buffers();
  // p_mask->smooth_overlap_buffers();

  post_process_heightmap(node, *p_out);
}

} // namespace hesiod
