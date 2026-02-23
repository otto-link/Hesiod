/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"
#include "highmap/virtual_array/virtual_array.hpp"

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

constexpr const char *P_OUTPUT = "phasor_fbm";
constexpr const char *P_ANGLE = "angle";
constexpr const char *P_NOISE_X = "noise_x";
constexpr const char *P_NOISE_Y = "noise_y";

constexpr const char *A_SEED = "seed";
constexpr const char *A_KP_GLOBAL = "kp_global";
constexpr const char *A_ANGLE_SHIFT = "angle_shift";
constexpr const char *A_OCTAVES = "octaves";
constexpr const char *A_WEIGHT = "weight";
constexpr const char *A_PERSISTENCE = "persistence";
constexpr const char *A_LACUNARITY = "lacunarity";
constexpr const char *A_PROFILE = "profile";
constexpr const char *A_DELTA = "delta";
constexpr const char *A_PHASE_SMOOTHING = "phase_smoothing";

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void setup_phasor_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, P_ANGLE);
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, P_NOISE_X);
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, P_NOISE_Y);
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, P_OUTPUT, CONFIG(node));

  // attribute(s)
  // clang-format off
  node.add_attr<FloatAttribute>(A_KP_GLOBAL, "kp_global", 16.f, 0.f, FLT_MAX);
  node.add_attr<FloatAttribute>(A_ANGLE_SHIFT, "angle_shift", 0.f, -180.f, 180.f, "{:.0f}°");
  node.add_attr<SeedAttribute>(A_SEED, "Seed");
  node.add_attr<EnumAttribute>(A_PROFILE, "Profile Type", enum_mappings.phasor_profile_map, "Cosine");
  node.add_attr<IntAttribute>(A_OCTAVES, "Octaves", 1, 1, 8);
  node.add_attr<FloatAttribute>(A_WEIGHT, "Weight", 0.7f, 0.f, 1.f);
  node.add_attr<FloatAttribute>(A_PERSISTENCE, "Persistence", 0.3f, 0.f, 1.f);
  node.add_attr<FloatAttribute>(A_LACUNARITY, "Lacunarity", 2.f, 0.01f, 4.f);
  node.add_attr<FloatAttribute>(A_DELTA, "delta", 0.01f, 0.f, 1.f);
  node.add_attr<FloatAttribute>(A_PHASE_SMOOTHING, "phase_smoothing", 10.f, 0.001f, 100.f, "{:.2e}", true);
  // clang-format on

  // attribute(s) order
  node.set_attr_ordered_key({A_KP_GLOBAL,
                             A_ANGLE_SHIFT,
                             A_SEED,
                             A_PROFILE,
                             A_OCTAVES,
                             A_WEIGHT,
                             A_PERSISTENCE,
                             A_LACUNARITY,
                             A_DELTA,
                             A_PHASE_SMOOTHING});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = false, .remap_active_state = true});
}

// -----------------------------------------------------------------------------
// Compute
// -----------------------------------------------------------------------------

void compute_phasor_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  auto p_angle = node.get_value_ref<hmap::VirtualArray>(P_ANGLE);
  auto p_noise_x = node.get_value_ref<hmap::VirtualArray>(P_NOISE_X);
  auto p_noise_y = node.get_value_ref<hmap::VirtualArray>(P_NOISE_Y);
  auto p_out = node.get_value_ref<hmap::VirtualArray>(P_OUTPUT);

  // --- Parameters wrapper

  const auto params = [&node]()
  {
    struct P
    {
      uint                seed;
      float               kp_global;
      float               angle_shift_rads;
      int                 octaves;
      float               weight;
      float               persistence;
      float               lacunarity;
      int                 n_kernel_samples;
      int                 angle_filter_ir;
      hmap::PhasorProfile profile;
      float               delta;
      float               phase_smoothing;
    };

    const auto ep = hmap::PhasorProfile(node.get_attr<EnumAttribute>(A_PROFILE));

    return P{
        .seed = node.get_attr<SeedAttribute>(A_SEED),
        .kp_global = node.get_attr<FloatAttribute>(A_KP_GLOBAL),
        .angle_shift_rads = float(M_PI) / 180.f *
                            node.get_attr<FloatAttribute>(A_ANGLE_SHIFT),
        .octaves = node.get_attr<IntAttribute>(A_OCTAVES),
        .weight = node.get_attr<FloatAttribute>(A_WEIGHT),
        .persistence = node.get_attr<FloatAttribute>(A_PERSISTENCE),
        .lacunarity = node.get_attr<FloatAttribute>(A_LACUNARITY),
        .n_kernel_samples = 8,
        .angle_filter_ir = 1,
        .profile = ep,
        .delta = node.get_attr<FloatAttribute>(A_DELTA),
        .phase_smoothing = node.get_attr<FloatAttribute>(A_PHASE_SMOOTHING),
    };
  }();

  // --- Compute

  hmap::for_each_tile(
      {p_noise_x, p_noise_y, p_angle},
      {p_out},
      [&node, &params](std::vector<const hmap::Array *> p_arrays_in,
                       std::vector<hmap::Array *>       p_arrays_out,
                       const hmap::TileRegion          &region)
      {
        auto [pa_noise_x, pa_noise_y, pa_angle] = unpack<3>(p_arrays_in);
        auto [pa_out] = unpack<1>(p_arrays_out);

        *pa_out = hmap::gpu::phasor_fbm(params.profile,
                                        region.shape,
                                        params.kp_global,
                                        params.seed,
                                        params.angle_shift_rads,
                                        params.octaves,
                                        params.weight,
                                        params.persistence,
                                        params.lacunarity,
                                        params.n_kernel_samples,
                                        {1.f, 1.f},
                                        params.angle_filter_ir,
                                        params.delta,
                                        params.phase_smoothing,
                                        pa_angle,
                                        pa_noise_x,
                                        pa_noise_y,
                                        region.bbox);
      },
      node.cfg().cm_gpu);

  // post-process
  post_process_heightmap(node, *p_out);
}

} // namespace hesiod
