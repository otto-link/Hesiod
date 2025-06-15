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
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_voronoi_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  ADD_ATTR(EnumAttribute,
           "return_type",
           voronoi_return_type_map,
           "F1: distance to the closest point");
  ADD_ATTR(WaveNbAttribute, "kw");
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(FloatAttribute, "jitter.x", 0.5f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "jitter.y", 0.5f, 0.f, 1.f);
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"return_type",
                                "_SEPARATOR_",
                                "kw",
                                "seed",
                                "jitter.x",
                                "jitter.y",
                                "_SEPARATOR_",
                                "inverse",
                                "remap"});
}

void compute_voronoi_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

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
                                     rtype,
                                     pa_ctrl,
                                     pa_dx,
                                     pa_dy,
                                     bbox);
      },
      p_node->get_config_ref()->hmap_transform_mode_gpu);

  // post-process
  post_apply_enveloppe(p_node, *p_out, p_env);

  post_process_heightmap(p_node,
                         *p_out,
                         GET("inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR("remap", RangeAttribute, is_active),
                         GET("remap", RangeAttribute));

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
