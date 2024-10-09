/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_noise_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  p_node->add_attr<MapEnumAttribute>("noise_type", noise_type_map, "Noise type");
  p_node->add_attr<WaveNbAttribute>("kw");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<BoolAttribute>("inverse", false, "Inverse");
  p_node->add_attr<RangeAttribute>("remap", "Remap range");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"noise_type", "_SEPARATOR_", "kw", "seed", "_SEPARATOR_", "inverse", "remap"});
}

void compute_noise_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::HeightMap *p_dx = p_node->get_value_ref<hmap::HeightMap>("dx");
  hmap::HeightMap *p_dy = p_node->get_value_ref<hmap::HeightMap>("dy");
  hmap::HeightMap *p_env = p_node->get_value_ref<hmap::HeightMap>("envelope");
  hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("out");

  hmap::fill(*p_out,
             p_dx,
             p_dy,
             [p_node](hmap::Vec2<int>   shape,
                      hmap::Vec4<float> bbox,
                      hmap::Array      *p_noise_x,
                      hmap::Array      *p_noise_y)
             {
               return hmap::noise((hmap::NoiseType)GET("noise_type", MapEnumAttribute),
                                  shape,
                                  GET("kw", WaveNbAttribute),
                                  GET("seed", SeedAttribute),
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
                         GET_ATTR("remap", RangeAttribute, is_active),
                         GET("remap", RangeAttribute));

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
