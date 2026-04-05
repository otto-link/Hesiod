/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/shortest_path.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

// -----------------------------------------------------------------------------
// Ports & Attributes
// -----------------------------------------------------------------------------

constexpr const char *P_PATH = "path";
constexpr const char *P_IN = "input";

constexpr const char *A_SEED = "seed";
constexpr const char *A_START = "start";
constexpr const char *A_END = "end";
constexpr const char *A_MIDP_ITERATIONS = "midp_iterations";
constexpr const char *A_MIDP_SIGMA = "midp_sigma";
constexpr const char *A_FAVOR_BOUNDARY_CENTER = "favor_boundary_center";
constexpr const char *A_FAVOR_LOWER_ELEVATION = "favor_lower_elevation";
constexpr const char *A_FAVOR_SINKS = "favor_sinks";

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void setup_find_cut_path_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, P_IN);
  node.add_port<hmap::Path>(gnode::PortType::OUT, P_PATH);

  // attribute(s)
  // clang-format off
  node.add_attr<EnumAttribute>(A_START, "Start Boundary", enum_mappings.domain_boundary_map, "West");
  node.add_attr<EnumAttribute>(A_END, "End Boundary", enum_mappings.domain_boundary_map, "East");
  node.add_attr<IntAttribute>(A_MIDP_ITERATIONS, "Midpoint Iterations", 4, 0, 8);
  node.add_attr<FloatAttribute>(A_MIDP_SIGMA, "Midpoint Radius", 0.2f, 0.f, 1.f);
  node.add_attr<SeedAttribute>(A_SEED, "Seed");
  node.add_attr<BoolAttribute>(A_FAVOR_BOUNDARY_CENTER, "Favor Boundary Center", true);
  node.add_attr<BoolAttribute>(A_FAVOR_LOWER_ELEVATION, "Favor Lower Elevation", true);
  node.add_attr<BoolAttribute>(A_FAVOR_SINKS, "Favor Sinks (Local Minima)", true);
  // clang-format on

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Boundary",
                             A_START,
                             A_END,
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Midpoint",
                             A_MIDP_ITERATIONS,
                             A_MIDP_SIGMA,
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Random",
                             A_SEED,
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Sampling Weights",
                             A_FAVOR_BOUNDARY_CENTER,
                             A_FAVOR_LOWER_ELEVATION,
                             A_FAVOR_SINKS,
                             "_GROUPBOX_END_"});
}

// -----------------------------------------------------------------------------
// Compute
// -----------------------------------------------------------------------------

void compute_find_cut_path_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  auto *p_in = node.get_value_ref<hmap::VirtualArray>(P_IN);
  auto *p_path = node.get_value_ref<hmap::Path>(P_PATH);

  if (!p_in)
    return;

  // --- Params lambda

  const auto params = [&node]()
  {
    struct P
    {
      hmap::DomainBoundary start;
      hmap::DomainBoundary end;
      uint                 seed;
      int                  midp_iterations;
      float                midp_sigma;
      bool                 favor_boundary_center;
      bool                 favor_lower_elevation;
      bool                 favor_sinks;
    };

    // clang-format off
    return P{
      .start = hmap::DomainBoundary(node.get_attr<EnumAttribute>(A_START)),
      .end = hmap::DomainBoundary(node.get_attr<EnumAttribute>(A_END)),
      .seed = node.get_attr<SeedAttribute>(A_SEED),
      .midp_iterations = node.get_attr<IntAttribute>(A_MIDP_ITERATIONS),
      .midp_sigma = node.get_attr<FloatAttribute>(A_MIDP_SIGMA),
      .favor_boundary_center = node.get_attr<BoolAttribute>(A_FAVOR_BOUNDARY_CENTER),
      .favor_lower_elevation = node.get_attr<BoolAttribute>(A_FAVOR_LOWER_ELEVATION),
      .favor_sinks = node.get_attr<BoolAttribute>(A_FAVOR_SINKS)
    };
    // clang-format on
  }();

  // --- Compute

  hmap::for_each_tile(
      {p_in},
      [&node, p_path, &params](std::vector<hmap::Array *> p_arrays,
                               const hmap::TileRegion &)
      {
        auto [pa_in] = unpack<1>(p_arrays);

        *p_path = hmap::find_cut_path_midpoint(*pa_in,
                                               params.start,
                                               params.end,
                                               params.seed,
                                               params.midp_iterations,
                                               params.midp_sigma,
                                               params.favor_boundary_center,
                                               params.favor_lower_elevation,
                                               params.favor_sinks);
      },
      node.cfg().cm_single_array); // forced, not tileable
}

} // namespace hesiod
