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

void setup_basalt_field_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  std::vector<float> kw = {5.f, 5.f};

  ADD_ATTR(WaveNbAttribute, "kw", kw, 0.f, FLT_MAX, true);
  ADD_ATTR(SeedAttribute, "seed");

  ADD_ATTR(FloatAttribute, "warp_kw", 4.f, 0.f, FLT_MAX);

  ADD_ATTR(FloatAttribute, "large_scale_warp_amp", 0.2f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "large_scale_gain", 8., 0.f, 10.f);
  ADD_ATTR(FloatAttribute, "large_scale_amp", 0.15f, 0.f, 1.f);

  ADD_ATTR(FloatAttribute, "medium_scale_kw_ratio", 3.f, 0.f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "medium_scale_warp_amp", 0.1f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "medium_scale_gain", 8.f, 0.f, 10.f);
  ADD_ATTR(FloatAttribute, "medium_scale_amp", 0.12f, 0.f, 0.2f);

  ADD_ATTR(FloatAttribute, "small_scale_kw_ratio", 5.f, 0.f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "small_scale_amp", 0.1f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "small_scale_overlay_amp", 0.005f, 0.f, 0.01f);

  ADD_ATTR(FloatAttribute, "rugosity_kw_ratio", 1.5f, 0.f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "rugosity_amp", 1.f, 0.f, 2.f);

  ADD_ATTR(BoolAttribute, "flatten_activate", true);
  ADD_ATTR(FloatAttribute, "flatten_kw_ratio", 0.5f, 0.f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "flatten_amp", 0.f, 0.f, 1.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"kw",
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

  setup_post_process_heightmap_attributes(p_node);

  // disable post-processing remap by default
  GET_REF("post_remap", RangeAttribute)->set_is_active(false);
  GET_REF("post_remap", RangeAttribute)->save_initial_state();
}

void compute_basalt_field_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // base noise function
  hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_env = p_node->get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("out");

  hmap::transform(
      {p_out, p_dx, p_dy},
      [p_node](std::vector<hmap::Array *> p_arrays,
               hmap::Vec2<int>            shape,
               hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_dx = p_arrays[1];
        hmap::Array *pa_dy = p_arrays[2];

        *pa_out = hmap::gpu::basalt_field(shape,
                                          GET("kw", WaveNbAttribute),
                                          GET("seed", SeedAttribute),
                                          GET("warp_kw", FloatAttribute),
                                          GET("large_scale_warp_amp", FloatAttribute),
                                          GET("large_scale_gain", FloatAttribute),
                                          GET("large_scale_amp", FloatAttribute),
                                          GET("medium_scale_kw_ratio", FloatAttribute),
                                          GET("medium_scale_warp_amp", FloatAttribute),
                                          GET("medium_scale_gain", FloatAttribute),
                                          GET("medium_scale_amp", FloatAttribute),
                                          GET("small_scale_kw_ratio", FloatAttribute),
                                          GET("small_scale_amp", FloatAttribute),
                                          GET("small_scale_overlay_amp", FloatAttribute),
                                          GET("rugosity_kw_ratio", FloatAttribute),
                                          GET("rugosity_amp", FloatAttribute),
                                          GET("flatten_activate", BoolAttribute),
                                          GET("flatten_kw_ratio", FloatAttribute),
                                          GET("flatten_amp", FloatAttribute),
                                          pa_dx,
                                          pa_dy,
                                          bbox);
      },
      p_node->get_config_ref()->hmap_transform_mode_gpu);

  // post-process
  post_apply_enveloppe(p_node, *p_out, p_env);
  post_process_heightmap(p_node, *p_out);

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
