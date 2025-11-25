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

void setup_basalt_field_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG(node));

  // attribute(s)
  std::vector<float> kw = {5.f, 5.f};
  node.add_attr<WaveNbAttribute>("kw", "Spatial Frequency", kw, 0.f, FLT_MAX, true);
  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<FloatAttribute>("warp_kw", "warp_kw", 4.f, 0.f, FLT_MAX);
  node.add_attr<FloatAttribute>("large_scale_warp_amp",
                                "large_scale_warp_amp",
                                0.2f,
                                0.f,
                                1.f);
  node.add_attr<FloatAttribute>("large_scale_gain", "large_scale_gain", 8., 0.f, 10.f);
  node.add_attr<FloatAttribute>("large_scale_amp", "large_scale_amp", 0.15f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("medium_scale_kw_ratio",
                                "medium_scale_kw_ratio",
                                3.f,
                                0.f,
                                FLT_MAX);
  node.add_attr<FloatAttribute>("medium_scale_warp_amp",
                                "medium_scale_warp_amp",
                                0.1f,
                                0.f,
                                1.f);
  node.add_attr<FloatAttribute>("medium_scale_gain", "medium_scale_gain", 8.f, 0.f, 10.f);
  node.add_attr<FloatAttribute>("medium_scale_amp", "medium_scale_amp", 0.12f, 0.f, 0.2f);
  node.add_attr<FloatAttribute>("small_scale_kw_ratio",
                                "small_scale_kw_ratio",
                                5.f,
                                0.f,
                                FLT_MAX);
  node.add_attr<FloatAttribute>("small_scale_amp", "small_scale_amp", 0.1f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("small_scale_overlay_amp",
                                "small_scale_overlay_amp",
                                0.005f,
                                0.f,
                                0.01f);
  node.add_attr<FloatAttribute>("rugosity_kw_ratio",
                                "rugosity_kw_ratio",
                                1.5f,
                                0.f,
                                FLT_MAX);
  node.add_attr<FloatAttribute>("rugosity_amp", "rugosity_amp", 1.f, 0.f, 2.f);
  node.add_attr<BoolAttribute>("flatten_activate", "flatten_activate", true);
  node.add_attr<FloatAttribute>("flatten_kw_ratio",
                                "flatten_kw_ratio",
                                0.5f,
                                0.f,
                                FLT_MAX);
  node.add_attr<FloatAttribute>("flatten_amp", "flatten_amp", 0.f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"kw",
                             "seed",
                             "_TEXT_Large-Scale Parameters",
                             "warp_kw",
                             "large_scale_warp_amp",
                             "large_scale_gain",
                             "large_scale_amp",
                             "_TEXT_Medium-Scale Parameters",
                             "medium_scale_kw_ratio",
                             "medium_scale_warp_amp",
                             "medium_scale_gain",
                             "medium_scale_amp",
                             "_TEXT_Small-Scale Parameters",
                             "small_scale_kw_ratio",
                             "small_scale_amp",
                             "small_scale_overlay_amp",
                             "_TEXT_Rugosity",
                             "rugosity_kw_ratio",
                             "rugosity_amp",
                             "_TEXT_Flattening",
                             "flatten_activate",
                             "flatten_kw_ratio",
                             "flatten_amp"});

  setup_post_process_heightmap_attributes(node);

  // disable post-processing remap by default
  node.get_attr_ref<RangeAttribute>("post_remap")->set_is_active(false);
  node.get_attr_ref<RangeAttribute>("post_remap")->save_initial_state();
}

void compute_basalt_field_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
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

        *pa_out = hmap::gpu::basalt_field(
            shape,
            node.get_attr<WaveNbAttribute>("kw"),
            node.get_attr<SeedAttribute>("seed"),
            node.get_attr<FloatAttribute>("warp_kw"),
            node.get_attr<FloatAttribute>("large_scale_warp_amp"),
            node.get_attr<FloatAttribute>("large_scale_gain"),
            node.get_attr<FloatAttribute>("large_scale_amp"),
            node.get_attr<FloatAttribute>("medium_scale_kw_ratio"),
            node.get_attr<FloatAttribute>("medium_scale_warp_amp"),
            node.get_attr<FloatAttribute>("medium_scale_gain"),
            node.get_attr<FloatAttribute>("medium_scale_amp"),
            node.get_attr<FloatAttribute>("small_scale_kw_ratio"),
            node.get_attr<FloatAttribute>("small_scale_amp"),
            node.get_attr<FloatAttribute>("small_scale_overlay_amp"),
            node.get_attr<FloatAttribute>("rugosity_kw_ratio"),
            node.get_attr<FloatAttribute>("rugosity_amp"),
            node.get_attr<BoolAttribute>("flatten_activate"),
            node.get_attr<FloatAttribute>("flatten_kw_ratio"),
            node.get_attr<FloatAttribute>("flatten_amp"),
            pa_dx,
            pa_dy,
            bbox);
      },
      node.get_config_ref()->hmap_transform_mode_gpu);

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);
}

} // namespace hesiod
