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
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  ADD_ATTR(node, IntAttribute, "subsampling", 4, 1, 8);
  ADD_ATTR(node, WaveNbAttribute, "kw", std::vector<float>(2, 8.f), 0.f, FLT_MAX);
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, FloatAttribute, "eps", 0.2f, 0.f, 1.f);
  ADD_ATTR(node, IntAttribute, "resolution", 1, 1, 8);
  ADD_ATTR(node, FloatAttribute, "displacement", 0.075f, 0.f, 0.2f);
  ADD_ATTR(node, IntAttribute, "primitives_resolution_steps", 3, 1, 8);
  ADD_ATTR(node, FloatAttribute, "slope_power", 1.f, 0.f, 2.f);
  ADD_ATTR(node, FloatAttribute, "noise_amplitude_proportion", 0.01f, 0.f, 1.f);
  ADD_ATTR(node, BoolAttribute, "inverse", false);
  ADD_ATTR(node, RangeAttribute, "remap");

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
  Q_EMIT node.compute_started(node.get_id());

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
                     GET(node, "kw", WaveNbAttribute),
                     GET(node, "seed", SeedAttribute),
                     ctrl_array,
                     GET(node, "eps", FloatAttribute),
                     GET(node, "resolution", IntAttribute),
                     GET(node, "displacement", FloatAttribute),
                     GET(node, "primitives_resolution_steps", IntAttribute),
                     GET(node, "slope_power", FloatAttribute),
                     GET(node, "noise_amplitude_proportion", FloatAttribute),
                     true, // add noise
                     0.5f, // overlap
                     p_noise_x,
                     p_noise_y,
                     nullptr,
                     bbox,
                     GET(node, "subsampling", IntAttribute));
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
                           GET(node, "inverse", BoolAttribute),
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_MEMBER(node, "remap", RangeAttribute, is_active),
                           GET(node, "remap", RangeAttribute));
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
