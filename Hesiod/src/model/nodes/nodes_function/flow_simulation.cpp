/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/hydrology.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_flow_simulation_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "elevation");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "depth_map");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "water_depth", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("water_depth",
                                "Initial Water Depth",
                                0.01f,
                                0.001f,
                                0.5f,
                                "{:.2e}",
                                /* log */ true);
  node.add_attr<FloatAttribute>("duration", "Simulation Duration", 0.2f, 0.f, 2.f);
  node.add_attr<FloatAttribute>("dry_out_ratio",
                                "Dry-Out Threshold Ratio",
                                0.01f,
                                0.f,
                                1.f);
  node.add_attr<BoolAttribute>("flux_diffusion", "Enable Flux Diffusion", true);
  node.add_attr<FloatAttribute>("flux_diffusion_strength",
                                "Flux Diffusion Strength",
                                0.01f,
                                0.f,
                                0.1f);
  node.add_attr<IntAttribute>("solver_stride", "Solver Iteration Stride", 8, 1, 128);

  std::vector<std::string> choices = {"Uniform", "Pulse"};
  node.add_attr<ChoiceAttribute>("predefined_depth_map",
                                 "Predefined Depth Map",
                                 choices,
                                 "Pulse");

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Water Setup",
                             "water_depth",
                             "predefined_depth_map",
                             "duration",
                             "dry_out_ratio",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Solver",
                             "flux_diffusion",
                             "flux_diffusion_strength",
                             "solver_stride",
                             "_GROUPBOX_END_"});
}

void compute_flow_simulation_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_z = node.get_value_ref<hmap::VirtualArray>("elevation");

  if (p_z)
  {
    hmap::VirtualArray *p_depth_map = node.get_value_ref<hmap::VirtualArray>("depth_map");
    hmap::VirtualArray *p_water_depth = node.get_value_ref<hmap::VirtualArray>(
        "water_depth");

    // override compute mode (but keep storage mode)
    hmap::ComputeMode cm = node.cfg().cm_gpu;
    cm.mode = hmap::ForEachMode::VA_SINGLE_ARRAY_STRIDED;
    cm.stride = node.get_attr<IntAttribute>("solver_stride");

    int iterations = int(node.get_attr<FloatAttribute>("duration") * p_z->shape.x /
                         cm.stride);

    // use the predefined depth map only if the dpeth map input is not
    // set
    hmap::VirtualArray dmap(CONFIG(node));

    if (!p_depth_map)
    {
      p_depth_map = &dmap;

      hmap::for_each_tile(
          {p_depth_map},
          [&node](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &region)
          {
            auto [pa_depth_map] = unpack<1>(p_arrays);
            const std::string type = node.get_attr<ChoiceAttribute>(
                "predefined_depth_map");

            if (type == "Uniform")
            {
              *pa_depth_map = 1.f;
            }
            else if (type == "Pulse")
            {
              *pa_depth_map = hmap::cubic_pulse(region.shape,
                                                /* p_noise_x */ nullptr,
                                                /* p_noise_y */ nullptr,
                                                /* center */ {0.5f, 0.5f},
                                                region.bbox);
            }
          },
          node.cfg().cm_cpu);
    }

    // inputs
    float dmin = p_depth_map->min(node.cfg().cm_cpu);
    float dmax = p_depth_map->max(node.cfg().cm_cpu);

    if (dmin == dmax)
    {
      dmin = 0.f;
      dmax = 1.f;
    }

    // compute
    hmap::for_each_tile(
        {p_z, p_depth_map},
        {p_water_depth},
        [&node, iterations, dmin, dmax](std::vector<const hmap::Array *> p_arrays_in,
                                        std::vector<hmap::Array *>       p_arrays_out,
                                        const hmap::TileRegion &)
        {
          auto [pa_z, pa_depth_map] = unpack<2>(p_arrays_in);
          auto [pa_water_depth] = unpack<1>(p_arrays_out);

          hmap::Array depth_map_scaled = *pa_depth_map;
          hmap::remap(depth_map_scaled, 0.f, 1.f, dmin, dmax);

          depth_map_scaled.dump();

          *pa_water_depth = hmap::gpu::flow_simulation(
              *pa_z,
              node.get_attr<FloatAttribute>("water_depth"),
              depth_map_scaled,
              iterations,
              /* dt */ 0.5f,
              node.get_attr<BoolAttribute>("flux_diffusion"),
              node.get_attr<FloatAttribute>("flux_diffusion_strength"),
              node.get_attr<FloatAttribute>("dry_out_ratio"));
        },
        cm);
  }
}

} // namespace hesiod
