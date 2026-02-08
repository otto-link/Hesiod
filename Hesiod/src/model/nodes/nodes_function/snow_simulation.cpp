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

void setup_snow_simulation_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "elevation_in");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "depth_map");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "snow_depth_in");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "melting_map");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "elevation", CONFIG(node));
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "snow_depth", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("snow_depth",
                                "Snow Amount",
                                0.1f,
                                0.001f,
                                0.5f,
                                "{:.2e}",
                                /* log */ true);
  node.add_attr<FloatAttribute>("talus_global", "Base Repose Slope", 1.5f, 0.f, FLT_MAX);

  node.add_attr<FloatAttribute>("duration", "Simulation Duration", 1.f, 0.f, 2.f);
  node.add_attr<IntAttribute>("solver_stride", "Solver Stride", 2, 1, 32);

  node.add_attr<BoolAttribute>("post_filter", "Enable Thermal Relaxation", true);
  node.add_attr<FloatAttribute>("thermal_talus_ratio",
                                "Thermal Repose Ratio",
                                0.2f,
                                0.01f,
                                1.f);

  node.add_attr<FloatAttribute>("k_snow", "Avalanche Strength", 0.5f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("k_visc", "Thermal Creep Strength", 0.1f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("k_melt_factor", "Melting Strength", 0.8f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("k_depth_ratio", "Depth Stiffening", 1.f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("k_depth_slope_ratio",
                                "Depth Repose Boost",
                                1.f,
                                0.f,
                                4.f);

  node.add_attr<BoolAttribute>("shift_to_zero", "Rebase Snow to Zero", true);

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Snow Deposition",
                             "snow_depth",
                             "talus_global",
                             "duration",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Snow Dynamics",
                             "k_snow",
                             "k_visc",
                             "k_depth_ratio",
                             "k_depth_slope_ratio",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Melting",
                             "k_melt_factor",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Thermal Relaxation",
                             "post_filter",
                             "thermal_talus_ratio",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Output Conditioning",
                             "shift_to_zero",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Solver",
                             "solver_stride",
                             "_GROUPBOX_END_"});
}

void compute_snow_simulation_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  auto *p_z = node.get_value_ref<hmap::VirtualArray>("elevation_in");

  if (p_z)
  {
    auto *p_depth_map = node.get_value_ref<hmap::VirtualArray>("depth_map");
    auto *p_snow_depth_in = node.get_value_ref<hmap::VirtualArray>("snow_depth_in");
    auto *p_snow_depth = node.get_value_ref<hmap::VirtualArray>("snow_depth");
    auto *p_melting_map = node.get_value_ref<hmap::VirtualArray>("melting_map");
    auto *p_z_out = node.get_value_ref<hmap::VirtualArray>("elevation");

    // --- Parameters wrapper

    const auto params = [&node, p_z]()
    {
      struct P
      {
        int   solver_stride;
        float talus_global;
        float snow_depth;
        float duration;
        float k_snow, k_visc, k_melt_factor;
        float k_depth_ratio, k_depth_slope_ratio;
        float thermal_talus_ratio;
        bool  post_filter;
        bool  shift_to_zero;
        //
        int   nx_strided;
        int   iterations;
        float talus;
      };

      const int nx_strided = int(float(p_z->shape.x) /
                                 node.get_attr<IntAttribute>("solver_stride"));
      const int iterations = int(node.get_attr<FloatAttribute>("duration") * nx_strided);
      const float talus = node.get_attr<FloatAttribute>("talus_global") / nx_strided;

      return P{
          .solver_stride = node.get_attr<IntAttribute>("solver_stride"),
          .talus_global = node.get_attr<FloatAttribute>("talus_global"),
          .snow_depth = node.get_attr<FloatAttribute>("snow_depth"),
          .duration = node.get_attr<FloatAttribute>("duration"),
          .k_snow = node.get_attr<FloatAttribute>("k_snow"),
          .k_visc = node.get_attr<FloatAttribute>("k_visc"),
          .k_melt_factor = node.get_attr<FloatAttribute>("k_melt_factor"),
          .k_depth_ratio = node.get_attr<FloatAttribute>("k_depth_ratio"),
          .k_depth_slope_ratio = node.get_attr<FloatAttribute>("k_depth_slope_ratio"),
          .thermal_talus_ratio = node.get_attr<FloatAttribute>("thermal_talus_ratio"),
          .post_filter = node.get_attr<BoolAttribute>("post_filter"),
          .shift_to_zero = node.get_attr<BoolAttribute>("shift_to_zero"),
          //
          .nx_strided = nx_strided,
          .iterations = iterations,
          .talus = talus};
    }();

    // --- Adjust compute mode (stride)

    // override compute mode (but keep storage mode)
    hmap::ComputeMode cm = node.cfg().cm_gpu;
    cm.mode = hmap::ForEachMode::VA_SINGLE_ARRAY_STRIDED;
    cm.stride = params.solver_stride;

    // --- Create talus virtual array

    hmap::VirtualArray talus_map = hmap::VirtualArray(CONFIG(node));
    talus_map.fill(params.talus, node.cfg().cm_cpu);

    // --- Create melting map if not available

    hmap::VirtualArray melting_map = hmap::VirtualArray(CONFIG(node));

    if (!p_melting_map)
    {
      p_melting_map = &melting_map;
      melting_map.fill(0.f, node.cfg().cm_cpu);
    }

    // --- Resolve depth map source

    // may be overriden when tne snow depth is set as an input
    float snow_depth_updated = params.snow_depth;

    hmap::VirtualArray dmap(CONFIG(node));

    // provided input snow depth has priority, if not use the a
    // uniform depth map if the depth map input is not set
    if (p_snow_depth_in)
    {
      copy_data(*p_snow_depth_in, dmap, node.cfg().cm_cpu);
      snow_depth_updated = dmap.max(node.cfg().cm_cpu);
      p_depth_map = &dmap;
    }
    else if (!p_depth_map)
    {
      dmap.fill(1.f, node.cfg().cm_cpu);
      p_depth_map = &dmap;
    }

    // --- Snow simulation

    // retrieve depth range for normalization
    float dmin = p_depth_map->min(node.cfg().cm_cpu);
    float dmax = p_depth_map->max(node.cfg().cm_cpu);

    if (dmin == dmax)
    {
      dmin = 0.f;
      dmax = 1.f;
    }

    // eventually compute
    hmap::for_each_tile(
        {p_z, p_depth_map, p_melting_map, &talus_map},
        {p_z_out, p_snow_depth},
        [&node, &params, dmin, dmax, snow_depth_updated](
            std::vector<const hmap::Array *> p_arrays_in,
            std::vector<hmap::Array *>       p_arrays_out,
            const hmap::TileRegion &)
        {
          const auto [pa_z, pa_depth_map, pa_melting_map, pa_talus_map] = unpack<4>(
              p_arrays_in);
          auto [pa_z_out, pa_snow_depth] = unpack<2>(p_arrays_out);

          hmap::Array depth_map_scaled = *pa_depth_map;
          hmap::remap(depth_map_scaled, 0.f, 1.f, dmin, dmax);

          *pa_snow_depth = hmap::gpu::snow_simulation(*pa_z,
                                                      snow_depth_updated,
                                                      *pa_depth_map,
                                                      *pa_melting_map,
                                                      *pa_talus_map,
                                                      params.iterations,
                                                      /* dt */ 0.5f,
                                                      /* fall_iterations_ratio */ 1.f,
                                                      params.k_snow,
                                                      params.k_visc,
                                                      params.k_melt_factor,
                                                      params.k_depth_ratio,
                                                      params.k_depth_slope_ratio,
                                                      params.post_filter,
                                                      params.thermal_talus_ratio);

          *pa_z_out = *pa_z + *pa_snow_depth;
        },
        cm);

    // --- clean-up output

    // force minimum snow depth to be actually "zero" (more
    // convenient)
    if (params.shift_to_zero)
      p_snow_depth->remap(0.f, p_snow_depth->max(node.cfg().cm_cpu), node.cfg().cm_cpu);
  }
}

} // namespace hesiod
