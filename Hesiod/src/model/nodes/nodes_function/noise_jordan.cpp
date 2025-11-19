/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/erosion.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_noise_jordan_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG(node));

  // attribute(s)
  node.add_attr<EnumAttribute>("noise_type",
                               "noise_type",
                               enum_mappings.noise_type_map_fbm);

  node.add_attr<WaveNbAttribute>("kw", "kw");

  node.add_attr<SeedAttribute>("seed", "seed");

  node.add_attr<IntAttribute>("octaves", "octaves", 8, 0, 32);

  node.add_attr<FloatAttribute>("weight", "weight", 0.7f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("persistence", "persistence", 0.5f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("lacunarity", "lacunarity", 2.f, 0.01f, 4.f);

  node.add_attr<FloatAttribute>("warp0", "warp0", 0.2f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("damp0", "damp0", 1.f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("warp_scale", "warp_scale", 0.2f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("damp_scale", "damp_scale", 1.f, 0.f, 1.f);

  node.add_attr<BoolAttribute>("inverse", "inverse", false);

  node.add_attr<RangeAttribute>("remap", "remap");

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
                             "warp0",
                             "damp0",
                             "warp_scale",
                             "damp_scale",
                             "_SEPARATOR_",
                             "inverse",
                             "remap"});
}

void compute_noise_jordan_node(BaseNode &node)
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
               return hmap::noise_jordan(
                   (hmap::NoiseType)node.get_attr<EnumAttribute>("noise_type"),
                   shape,
                   node.get_attr<WaveNbAttribute>("kw"),
                   node.get_attr<SeedAttribute>("seed"),
                   node.get_attr<IntAttribute>("octaves"),
                   node.get_attr<FloatAttribute>("weight"),
                   node.get_attr<FloatAttribute>("persistence"),
                   node.get_attr<FloatAttribute>("lacunarity"),
                   node.get_attr<FloatAttribute>("warp0"),
                   node.get_attr<FloatAttribute>("damp0"),
                   node.get_attr<FloatAttribute>("warp_scale"),
                   node.get_attr<FloatAttribute>("damp_scale"),
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

                         node.get_attr<BoolAttribute>("inverse"),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         node.get_attr_ref<RangeAttribute>("remap")->get_is_active(),
                         node.get_attr<RangeAttribute>("remap"));

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
