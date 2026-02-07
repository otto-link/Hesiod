/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/filters.hpp"
#include "highmap/math.hpp"
#include "highmap/multiscale/downscaling.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_particle_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "bedrock");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "moisture");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "mask");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "erosion", CONFIG(node));
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "deposition", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("scale", "scale", 0.5f, 0.1f, 1.f);
  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<FloatAttribute>("particle_density", "particle_density", 1.f, 0.f, 4.f);
  node.add_attr<FloatAttribute>("c_capacity", "c_capacity", 10.f, 0.1f, 40.f);
  node.add_attr<FloatAttribute>("c_erosion", "c_erosion", 0.3f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("c_deposition", "c_deposition", 0.3f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("c_inertia", "c_inertia", 0.3f, 0.01f, 2.f);
  node.add_attr<FloatAttribute>("c_gravity", "c_gravity", 1.f, 0.01f, 10.f);
  node.add_attr<IntAttribute>("radius", "radius", 2, 2, 8);
  node.add_attr<FloatAttribute>("drag_rate", "drag_rate", 0., 0.f, 0.02f);
  node.add_attr<FloatAttribute>("evap_rate", "evap_rate", 0.01f, 0.f, 0.05f);
  node.add_attr<BoolAttribute>("post_filtering", "post_filtering", true);
  node.add_attr<BoolAttribute>("post_filtering_local", "post_filtering_local", false);
  node.add_attr<BoolAttribute>("deposition_only", "deposition_only", false);

  // attribute(s) order
  node.set_attr_ordered_key({"scale",
                             "seed",
                             "particle_density",
                             "c_capacity",
                             "c_erosion",
                             "c_deposition",
                             "c_inertia",
                             "c_gravity",
                             "radius",
                             "drag_rate",
                             "evap_rate",
                             "post_filtering",
                             "post_filtering_local",
                             "deposition_only"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = true});
}

void compute_hydraulic_particle_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  if (p_in)
  {
    hmap::VirtualArray *p_bedrock = node.get_value_ref<hmap::VirtualArray>("bedrock");
    hmap::VirtualArray *p_moisture_map = node.get_value_ref<hmap::VirtualArray>(
        "moisture");
    hmap::VirtualArray *p_mask = node.get_value_ref<hmap::VirtualArray>("mask");

    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");
    hmap::VirtualArray *p_erosion_map = node.get_value_ref<hmap::VirtualArray>("erosion");
    hmap::VirtualArray *p_deposition_map = node.get_value_ref<hmap::VirtualArray>(
        "deposition");

    // set the bedrock at the input heightmap to prevent any erosion,
    // if requested
    if (node.get_attr<BoolAttribute>("deposition_only"))
      p_bedrock = p_in;

    const float scale = node.get_attr<FloatAttribute>("scale");

    // number of particles based on the input particle density
    int nparticles = (int)(node.get_attr<FloatAttribute>("particle_density") *
                           p_out->shape.x * p_out->shape.y * scale);

    // override compute mode (but keep storage mode)
    hmap::ComputeMode cm = node.cfg().cm_gpu;

    if (scale != 1.f)
    {
      cm.mode = hmap::ForEachMode::VA_SINGLE_ARRAY_DOWNSCALED;
      cm.k_cutoff = scale;
    }

    hmap::copy_data(*p_in, *p_out, node.cfg().cm_cpu);
    p_erosion_map->fill(0.f, node.cfg().cm_cpu);
    p_deposition_map->fill(0.f, node.cfg().cm_cpu);

    hmap::for_each_tile(
        {p_bedrock, p_moisture_map, p_mask},
        {p_out, p_erosion_map, p_deposition_map},
        [&node, &nparticles](std::vector<const hmap::Array *> p_arrays_in,
                             std::vector<hmap::Array *>       p_arrays_out,
                             const hmap::TileRegion &)
        {
          auto [pa_bedrock, pa_moisture_map, pa_mask] = unpack<3>(p_arrays_in);
          auto [pa_out, pa_erosion_map, pa_deposition_map] = unpack<3>(p_arrays_out);

          // add hard limit
          hmap::Array bedrock_limit;
          if (!pa_bedrock)
          {
            bedrock_limit = *pa_out - 0.1f;
            pa_bedrock = &bedrock_limit;
          }

          hmap::gpu::hydraulic_particle(*pa_out,
                                        pa_mask,
                                        nparticles,
                                        node.get_attr<SeedAttribute>("seed"),
                                        pa_bedrock,
                                        pa_moisture_map,
                                        pa_erosion_map,
                                        pa_deposition_map,
                                        node.get_attr<FloatAttribute>("c_capacity"),
                                        node.get_attr<FloatAttribute>("c_erosion"),
                                        node.get_attr<FloatAttribute>("c_deposition"),
                                        node.get_attr<FloatAttribute>("c_inertia"),
                                        node.get_attr<FloatAttribute>("c_gravity"),
                                        node.get_attr<IntAttribute>("radius"),
                                        node.get_attr<FloatAttribute>("drag_rate"),
                                        node.get_attr<FloatAttribute>("evap_rate"),
                                        node.get_attr<BoolAttribute>("post_filtering"));
        },
        cm);

    // post-traatments
    p_out->smooth_overlap_buffers();

    p_erosion_map->smooth_overlap_buffers();
    p_erosion_map->remap(0.f, 1.f, node.cfg().cm_cpu);

    p_deposition_map->smooth_overlap_buffers();
    p_deposition_map->remap(0.f, 1.f, node.cfg().cm_cpu);

    // clean-up erosion and deposition surfaces
    if (node.get_attr<BoolAttribute>("post_filtering_local"))
    {
      hmap::for_each_tile(
          {p_out, p_erosion_map, p_deposition_map},
          [&node, nparticles](std::vector<hmap::Array *> p_arrays,
                              const hmap::TileRegion &)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_erosion_map = p_arrays[1];
            hmap::Array *pa_deposition_map = p_arrays[2];

            // TODO harcoded parameters
            hmap::Array mask = hmap::maximum_smooth(*pa_erosion_map,
                                                    *pa_deposition_map,
                                                    0.05f);
            mask = hmap::pow(mask, 0.5f);
            hmap::gpu::smooth_cpulse(mask, 2);
            hmap::gpu::smooth_cpulse(*pa_out, 32, &mask);
          },
          node.cfg().cm_gpu);
    }

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }
}

} // namespace hesiod
