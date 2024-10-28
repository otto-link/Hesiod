/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_noise_swiss_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "control");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<MapEnumAttribute>("noise_type", noise_type_map_fbm, "noise_type");
  p_node->add_attr<WaveNbAttribute>("kw");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<IntAttribute>("octaves", 8, 0, 32, "octaves");
  p_node->add_attr<FloatAttribute>("weight", 0.7f, 0.f, 1.f, "weight");
  p_node->add_attr<FloatAttribute>("persistence", 0.5f, 0.f, 1.f, "persistence");
  p_node->add_attr<FloatAttribute>("lacunarity", 2.f, 0.01f, 4.f, "lacunarity");
  p_node->add_attr<FloatAttribute>("warp_scale", 0.1f, 0.f, 0.5f, "warp_scale");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<RangeAttribute>("remap_range", "remap_range");

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
                                "warp_scale",
                                "_SEPARATOR_",
                                "inverse",
                                "remap_range"});
}

void compute_noise_swiss_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::HeightMap *p_dx = p_node->get_value_ref<hmap::HeightMap>("dx");
  hmap::HeightMap *p_dy = p_node->get_value_ref<hmap::HeightMap>("dy");
  hmap::HeightMap *p_ctrl = p_node->get_value_ref<hmap::HeightMap>("control");
  hmap::HeightMap *p_env = p_node->get_value_ref<hmap::HeightMap>("envelope");
  hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

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
               return hmap::noise_swiss(
                   (hmap::NoiseType)GET("noise_type", MapEnumAttribute),
                   shape,
                   GET("kw", WaveNbAttribute),
                   GET("seed", SeedAttribute),
                   GET("octaves", IntAttribute),
                   GET("weight", FloatAttribute),
                   GET("persistence", FloatAttribute),
                   GET("lacunarity", FloatAttribute),
                   GET("warp_scale", FloatAttribute),
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
  post_process_heightmap(*p_out,
                         GET("inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR("remap_range", RangeAttribute, is_active),
                         GET("remap_range", RangeAttribute));

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod