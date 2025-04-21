/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/erosion.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_noise_jordan_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  ADD_ATTR(EnumAttribute, "noise_type", noise_type_map_fbm);
  ADD_ATTR(WaveNbAttribute, "kw");
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(IntAttribute, "octaves", 8, 0, 32);
  ADD_ATTR(FloatAttribute, "weight", 0.7f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "persistence", 0.5f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "lacunarity", 2.f, 0.01f, 4.f);
  ADD_ATTR(FloatAttribute, "warp0", 0.2f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "damp0", 1.f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "warp_scale", 0.2f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "damp_scale", 1.f, 0.f, 1.f);
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"noise_type",
                                "_SEPARATOR_",
                                "kw",
                                "seed",
                                "octaves",
                                "weight",
                                "persistence",
                                "lacunarity",
                                "_SEPARATOR_",
                                "warp0",
                                "damp0",
                                "warp_scale",
                                "damp_scale",
                                "_SEPARATOR_",
                                "inverse",
                                "remap"});
}

void compute_noise_jordan_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = p_node->get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_env = p_node->get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

  hmap::fill(*p_out,
             p_dx,
             p_dy,
             p_ctrl,
             [p_node](hmap::Vec2<int>   shape,
                      hmap::Vec4<float> bbox,
                      hmap::Array      *p_noise_x,
                      hmap::Array      *p_noise_y,
                      hmap::Array      *p_ctrl)
             {
               return hmap::noise_jordan(
                   (hmap::NoiseType)GET("noise_type", EnumAttribute),
                   shape,
                   GET("kw", WaveNbAttribute),
                   GET("seed", SeedAttribute),
                   GET("octaves", IntAttribute),
                   GET("weight", FloatAttribute),
                   GET("persistence", FloatAttribute),
                   GET("lacunarity", FloatAttribute),
                   GET("warp0", FloatAttribute),
                   GET("damp0", FloatAttribute),
                   GET("warp_scale", FloatAttribute),
                   GET("damp_scale", FloatAttribute),
                   p_ctrl,
                   p_noise_x,
                   p_noise_y,
                   nullptr,
                   bbox);
             });

  // add envelope
  if (p_env)
  {
    float hmin = p_out->min();
    hmap::transform(*p_out,
                    *p_env,
                    [&hmin](hmap::Array &a, hmap::Array &b)
                    {
                      a -= hmin;
                      a *= b;
                    });
  }

  // post-process
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
