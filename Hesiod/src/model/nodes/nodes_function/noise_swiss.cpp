/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_noise_swiss_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, EnumAttribute, "noise_type", enum_mappings.noise_type_map_fbm);
  ADD_ATTR(node, WaveNbAttribute, "kw");
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, IntAttribute, "octaves", 8, 0, 32);
  ADD_ATTR(node, FloatAttribute, "weight", 0.7f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "persistence", 0.5f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "lacunarity", 2.f, 0.01f, 4.f);
  ADD_ATTR(node, FloatAttribute, "warp_scale", 0.1f, 0.f, 0.5f);
  ADD_ATTR(node, BoolAttribute, "inverse", false);
  ADD_ATTR(node, RangeAttribute, "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"noise_type",
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
                             "remap"});
}

void compute_noise_swiss_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = node.get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  hmap::fill(*p_out,
             p_dx,
             p_dy,
             p_ctrl,
             [&node](hmap::Vec2<int>   shape,
                     hmap::Vec4<float> bbox,
                     hmap::Array      *p_noise_x,
                     hmap::Array      *p_noise_y,
                     hmap::Array      *p_ctrl)
             {
               return hmap::noise_swiss(
                   (hmap::NoiseType)GET(node, "noise_type", EnumAttribute),
                   shape,
                   GET(node, "kw", WaveNbAttribute),
                   GET(node, "seed", SeedAttribute),
                   GET(node, "octaves", IntAttribute),
                   GET(node, "weight", FloatAttribute),
                   GET(node, "persistence", FloatAttribute),
                   GET(node, "lacunarity", FloatAttribute),
                   GET(node, "warp_scale", FloatAttribute),
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
  post_process_heightmap(node,
                         *p_out,

                         GET(node, "inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_MEMBER(node, "remap", RangeAttribute, is_active),
                         GET(node, "remap", RangeAttribute));

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
