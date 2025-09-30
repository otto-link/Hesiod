/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/heightmap.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_shattered_peak_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "elevation", 0.7f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "scale", 1.f, 0.01f, FLT_MAX);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(FloatAttribute, "peak_kw", 4.f, 0.01f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "rugosity", 0.f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "angle", 45.f, -180.f, 180.f, "{:.0f}°");
  ADD_ATTR(FloatAttribute, "gamma", 1.f, 0.01f, 4.f);
  ADD_ATTR(BoolAttribute, "add_deposition", true);
  ADD_ATTR(FloatAttribute, "bulk_amp", 0.25f, 0.f, 2.f);
  ADD_ATTR(FloatAttribute, "base_noise_amp", 0.15f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "k_smoothing", 0.1f, 0.f, 1.f);
  ADD_ATTR(Vec2FloatAttribute, "center");

  // attribute(s) order
  p_node->set_attr_ordered_key({"elevation",
                                "scale",
                                "seed",
                                "peak_kw",
                                "_SEPARATOR_",
                                "rugosity",
                                "angle",
                                "k_smoothing",
                                "gamma",
                                "add_deposition",
                                "_SEPARATOR_",
                                "base_noise_amp",
                                "bulk_amp",
                                "center"});

  setup_post_process_heightmap_attributes(p_node);

  // disable post-processing remap by default
  GET_REF("post_remap", RangeAttribute)->set_is_active(false);
  GET_REF("post_remap", RangeAttribute)->save_initial_state();

  add_wip_warning_label(p_node);
}

void compute_shattered_peak_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // base shattered_peak function
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

        *pa_out = hmap::gpu::shattered_peak(shape,
                                            GET("seed", SeedAttribute),
                                            GET("scale", FloatAttribute),
                                            GET("peak_kw", FloatAttribute),
                                            GET("rugosity", FloatAttribute),
                                            GET("angle", FloatAttribute),
                                            GET("gamma", FloatAttribute),
                                            GET("add_deposition", BoolAttribute),
                                            GET("bulk_amp", FloatAttribute),
                                            GET("base_noise_amp", FloatAttribute),
                                            GET("k_smoothing", FloatAttribute),
                                            GET("center", Vec2FloatAttribute),
                                            pa_dx,
                                            pa_dy,
                                            bbox);
      },
      p_node->get_config_ref()->hmap_transform_mode_gpu);

  p_out->remap(0.f, GET("elevation", FloatAttribute));

  // post-process
  post_apply_enveloppe(p_node, *p_out, p_env);
  post_process_heightmap(p_node, *p_out);

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
