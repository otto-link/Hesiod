/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/heightmap.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_voronoi_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  ADD_ATTR(EnumAttribute,
           "return_type",
           HSD_CTX.enum_mappings.voronoi_return_type_map,
           "F1: squared distance to the closest point");
  ADD_ATTR(WaveNbAttribute, "kw");
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(FloatAttribute, "jitter.x", 1.f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "jitter.y", 1.f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "k_smoothing", 0.f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "exp_sigma", 0.1f, 0.f, 0.3f);
  ADD_ATTR(BoolAttribute, "sqrt_output", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"return_type",
                                "_SEPARATOR_",
                                "kw",
                                "seed",
                                "jitter.x",
                                "jitter.y",
                                "k_smoothing",
                                "exp_sigma",
                                "sqrt_output"});

  setup_post_process_heightmap_attributes(p_node);
}

void compute_voronoi_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // base noise function
  hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = p_node->get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_env = p_node->get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("out");

  hmap::transform(
      {p_out, p_ctrl, p_dx, p_dy},
      [p_node](std::vector<hmap::Array *> p_arrays,
               hmap::Vec2<int>            shape,
               hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_ctrl = p_arrays[1];
        hmap::Array *pa_dx = p_arrays[2];
        hmap::Array *pa_dy = p_arrays[3];

        hmap::VoronoiReturnType rtype = (hmap::VoronoiReturnType)GET("return_type",
                                                                     EnumAttribute);

        hmap::Vec2<float> jitter(GET("jitter.x", FloatAttribute),
                                 GET("jitter.y", FloatAttribute));

        *pa_out = hmap::gpu::voronoi(shape,
                                     GET("kw", WaveNbAttribute),
                                     GET("seed", SeedAttribute),
                                     jitter,
                                     GET("k_smoothing", FloatAttribute),
                                     GET("exp_sigma", FloatAttribute),
                                     rtype,
                                     pa_ctrl,
                                     pa_dx,
                                     pa_dy,
                                     bbox);
      },
      p_node->get_config_ref()->hmap_transform_mode_gpu);

  // apply square root
  p_out->remap();

  if (GET("sqrt_output", BoolAttribute))
    hmap::transform(
        {p_out},
        [](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          *pa_out = hmap::sqrt(*pa_out);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

  // post-process
  post_apply_enveloppe(p_node, *p_out, p_env);
  post_process_heightmap(p_node, *p_out);

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
