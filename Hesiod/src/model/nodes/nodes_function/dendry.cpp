/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_dendry_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG(node));

  // attribute(s)
  node.add_attr<IntAttribute>("subsampling", "subsampling", 4, 1, 8);

  node.add_attr<WaveNbAttribute>("kw",
                                 "Spatial Frequency",
                                 std::vector<float>(2, 8.f),
                                 0.f,
                                 FLT_MAX);
  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<FloatAttribute>("eps", "eps", 0.2f, 0.f, 1.f);
  node.add_attr<IntAttribute>("resolution", "resolution", 1, 1, 8);
  node.add_attr<FloatAttribute>("displacement", "displacement", 0.075f, 0.f, 0.2f);
  node.add_attr<IntAttribute>("primitives_resolution_steps",
                              "primitives_resolution_steps",
                              3,
                              1,
                              8);
  node.add_attr<FloatAttribute>("slope_power", "slope_power", 1.f, 0.f, 2.f);
  node.add_attr<FloatAttribute>("noise_amplitude_proportion",
                                "noise_amplitude_proportion",
                                0.01f,
                                0.f,
                                1.f);
  node.add_attr<BoolAttribute>("inverse", "inverse", false);
  node.add_attr<RangeAttribute>("remap", "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"subsampling",
                             "_SEPARATOR_",
                             "kw",
                             "seed",
                             "eps",
                             "slope_power",
                             "resolution",
                             "displacement",
                             "primitives_resolution_steps",
                             "noise_amplitude_proportion",
                             "_SEPARATOR_",
                             "inverse",
                             "remap"});
}

void compute_dendry_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = node.get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("out");

  if (p_ctrl)
  {
    // TODO shape
    hmap::Array ctrl_array = p_ctrl->to_array(hmap::Vec2<int>(128, 128));

    hmap::fill(*p_out,
               p_dx,
               p_dy,
               [&node, &ctrl_array](hmap::Vec2<int>   shape,
                                    hmap::Vec4<float> bbox,
                                    hmap::Array      *p_noise_x,
                                    hmap::Array      *p_noise_y)
               {
                 return hmap::dendry(
                     shape,
                     node.get_attr<WaveNbAttribute>("kw"),
                     node.get_attr<SeedAttribute>("seed"),
                     ctrl_array,
                     node.get_attr<FloatAttribute>("eps"),
                     node.get_attr<IntAttribute>("resolution"),
                     node.get_attr<FloatAttribute>("displacement"),
                     node.get_attr<IntAttribute>("primitives_resolution_steps"),
                     node.get_attr<FloatAttribute>("slope_power"),
                     node.get_attr<FloatAttribute>("noise_amplitude_proportion"),
                     true, // add noise
                     0.5f, // overlap
                     p_noise_x,
                     p_noise_y,
                     nullptr,
                     bbox,
                     node.get_attr<IntAttribute>("subsampling"));
               });

    p_out->smooth_overlap_buffers();

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
  }
}

} // namespace hesiod
