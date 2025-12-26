/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/heightmap.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_shattered_peak_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("elevation", "elevation", 0.7f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("scale", "scale", 1.f, 0.01f, FLT_MAX);
  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<IntAttribute>("octaves", "Octaves", 8, 0, 32);
  node.add_attr<FloatAttribute>("peak_kw", "peak_kw", 4.f, 0.01f, FLT_MAX);
  node.add_attr<FloatAttribute>("rugosity", "rugosity", 0.f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("angle", "angle", 45.f, -180.f, 180.f, "{:.0f}°");
  node.add_attr<FloatAttribute>("gamma", "gamma", 1.f, 0.01f, 4.f);
  node.add_attr<BoolAttribute>("add_deposition", "add_deposition", true);
  node.add_attr<FloatAttribute>("bulk_amp", "bulk_amp", 0.25f, 0.f, 2.f);
  node.add_attr<FloatAttribute>("base_noise_amp", "base_noise_amp", 0.15f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("k_smoothing", "k_smoothing", 0.1f, 0.f, 1.f);
  node.add_attr<Vec2FloatAttribute>("center", "center");

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Main Parameters",
                             "_TEXT_Base Terrain Shape",
                             "elevation",
                             "scale",
                             "seed",
                             "octaves",
                             "peak_kw",
                             "bulk_amp",
                             "_TEXT_Position",
                             "center",
                             //
                             "_TEXT_Surface Detail & Structure",
                             "rugosity",
                             "angle",
                             "k_smoothing",
                             "gamma",
                             "add_deposition",
                             //
                             "_TEXT_Noise & Modulation",
                             "base_noise_amp",
                             "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = false});

  // disable post-processing remap by default
  node.get_attr_ref<RangeAttribute>("post_remap")->set_is_active(false);
  node.get_attr_ref<RangeAttribute>("post_remap")->save_initial_state();
}

void compute_shattered_peak_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base shattered_peak function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("out");

  hmap::transform(
      {p_out, p_dx, p_dy},
      [&node](std::vector<hmap::Array *> p_arrays,
              hmap::Vec2<int>            shape,
              hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_dx = p_arrays[1];
        hmap::Array *pa_dy = p_arrays[2];

        *pa_out = hmap::gpu::shattered_peak(
            shape,
            node.get_attr<SeedAttribute>("seed"),
            node.get_attr<FloatAttribute>("scale"),
            node.get_attr<IntAttribute>("octaves"),
            node.get_attr<FloatAttribute>("peak_kw"),
            node.get_attr<FloatAttribute>("rugosity"),
            node.get_attr<FloatAttribute>("angle"),
            node.get_attr<FloatAttribute>("gamma"),
            node.get_attr<BoolAttribute>("add_deposition"),
            node.get_attr<FloatAttribute>("bulk_amp"),
            node.get_attr<FloatAttribute>("base_noise_amp"),
            node.get_attr<FloatAttribute>("k_smoothing"),
            node.get_attr<Vec2FloatAttribute>("center"),
            pa_dx,
            pa_dy,
            bbox);
      },
      node.get_config_ref()->hmap_transform_mode_gpu);

  p_out->remap(0.f, node.get_attr<FloatAttribute>("elevation"));

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);
}

} // namespace hesiod
