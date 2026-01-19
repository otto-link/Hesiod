/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "highmap/dbg/timer.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_valley_fill_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "noise");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "mask");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "deposition", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("talus_global", "Slope", 1.f, 0.f, FLT_MAX);
  node.add_attr<FloatAttribute>("duration", "Duration", 2.f, 0.05f, 6.f);
  node.add_attr<FloatAttribute>("ratio", "Deposition Ratio", 0.8f, 0.f, 1.f);
  node.add_attr<BoolAttribute>("preserve_elevation_range", "Preserve Input Range", true);
  node.add_attr<FloatAttribute>("gamma", "Deposition Gamma", 2.f, 0.01f, 4.f);
  node.add_attr<BoolAttribute>("scale_talus_with_elevation",
                               "Scale with Elevation",
                               true);
  node.add_attr<FloatAttribute>("elevation_max_ratio",
                                "Scree Max Elevation",
                                0.7f,
                                0.f,
                                2.f);
  node.add_attr<BoolAttribute>("add_default_noise", "Add Default Noise", true);
  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<FloatAttribute>("noise_amp", "Amplitude", 1.f, 0.f, 10.f);
  node.add_attr<FloatAttribute>("kw", "Spatial Frequency", 32.f, 0.f, FLT_MAX);

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Slope Constraints",
                             "talus_global",
                             "scale_talus_with_elevation",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Deposition Profile",
                             "ratio",
                             "gamma",
                             "elevation_max_ratio",
                             "preserve_elevation_range",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Deposition Dynamics",
                             "duration",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Noise Parameters",
                             "add_default_noise",
                             "seed",
                             "noise_amp",
                             "kw",
                             "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = false});
}

void compute_valley_fill_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  if (p_in)
  {
    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");
    hmap::VirtualArray *p_noise = node.get_value_ref<hmap::VirtualArray>("noise");
    hmap::VirtualArray *p_mask = node.get_value_ref<hmap::VirtualArray>("mask");
    hmap::VirtualArray *p_deposition_map = node.get_value_ref<hmap::VirtualArray>(
        "deposition");

    // --- prepapre talus field

    float talus = node.get_attr<FloatAttribute>("talus_global") / (float)p_out->shape.x;
    int   iterations = int(node.get_attr<FloatAttribute>("duration") * p_out->shape.x);

    hmap::VirtualArray talus_map = hmap::VirtualArray(CONFIG(node));
    talus_map.fill(talus, node.cfg().cm_cpu);

    if (node.get_attr<BoolAttribute>("scale_talus_with_elevation"))
    {
      talus_map.copy_from(*p_in, node.cfg().cm_cpu);
      talus_map.remap(talus / 10.f, talus, node.cfg().cm_cpu);
    }

    // --- generate default noise

    hmap::VirtualArray noise_default;

    if (!p_noise && node.get_attr<BoolAttribute>("add_default_noise"))
    {
      // set config
      noise_default.copy_from(*p_in, node.cfg().cm_cpu, /* copy_data */ false);

      hmap::for_each_tile(
          {&noise_default},
          [&node](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &region)
          {
            auto [pa_noise_default] = unpack<1>(p_arrays);

            hmap::Vec2<float> kw = {node.get_attr<FloatAttribute>("kw"),
                                    node.get_attr<FloatAttribute>("kw")};

            *pa_noise_default = hmap::gpu::noise_fbm(hmap::NoiseType::SIMPLEX2,
                                                     region.shape,
                                                     kw,
                                                     node.get_attr<SeedAttribute>("seed"),
                                                     8,
                                                     0.7f,
                                                     0.5f,
                                                     2.f,
                                                     nullptr,
                                                     nullptr,
                                                     nullptr,
                                                     nullptr,
                                                     region.bbox);

            pa_noise_default->dump();

            *pa_noise_default *= node.get_attr<FloatAttribute>("noise_amp");
          },
          node.cfg().cm_gpu);

      p_noise = &noise_default;

      Logger::log()->trace("\n{}", p_noise->info_string(node.cfg().cm_cpu));
    }

    // --- execute

    float zmin = p_in->min(node.cfg().cm_cpu);
    float zmax = p_in->max(node.cfg().cm_cpu);

    hmap::for_each_tile(
        {p_out, p_in, p_noise, p_mask, &talus_map, p_deposition_map},
        [&node, talus, iterations, zmin, zmax](std::vector<hmap::Array *> p_arrays,
                                               const hmap::TileRegion &)
        {
          auto [pa_out,
                pa_in,
                pa_noise,
                pa_mask,
                pa_talus_map,
                pa_deposition_map] = unpack<6>(p_arrays);

          *pa_out = *pa_in;

          hmap::gpu::valley_fill(*pa_out,
                                 pa_mask,
                                 *pa_talus_map,
                                 iterations,
                                 node.get_attr<FloatAttribute>("gamma"),
                                 node.get_attr<FloatAttribute>("ratio"),
                                 zmin,
                                 zmax,
                                 node.get_attr<FloatAttribute>("elevation_max_ratio"),
                                 node.get_attr<BoolAttribute>("preserve_elevation_range"),
                                 pa_noise,
                                 pa_deposition_map);
        },
        node.cfg().cm_gpu);

    // post-process
    p_out->smooth_overlap_buffers();
    post_process_heightmap(node, *p_out, p_in);

    p_deposition_map->remap(0.f, 1.f, node.cfg().cm_cpu);
  }
}

} // namespace hesiod
