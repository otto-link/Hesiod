/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_dendry_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "control");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  p_node->add_attr<IntAttribute>("subsampling", 4, 1, 8, "subsampling");
  p_node->add_attr<WaveNbAttribute>("kw", std::vector<float>(2, 8.f), 0.f, 32.f, "kw");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<FloatAttribute>("eps", 0.2f, 0.f, 1.f, "eps");
  p_node->add_attr<IntAttribute>("resolution", 1, 1, 8, "resolution");
  p_node->add_attr<FloatAttribute>("displacement", 0.075f, 0.f, 0.2f, "displacement");
  p_node->add_attr<IntAttribute>("primitives_resolution_steps",
                                 3,
                                 1,
                                 8,
                                 "primitives_resolution_steps");
  p_node->add_attr<FloatAttribute>("slope_power", 1.f, 0.f, 2.f, "slope_power");
  p_node->add_attr<FloatAttribute>("noise_amplitude_proportion",
                                   0.01f,
                                   0.f,
                                   1.f,
                                   "noise_amplitude_proportion");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<RangeAttribute>("remap_range", "remap_range");

  // attribute(s) order
  p_node->set_attr_ordered_key({"subsampling",
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
                                "remap_range"});
}

void compute_dendry_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::HeightMap *p_dx = p_node->get_value_ref<hmap::HeightMap>("dx");
  hmap::HeightMap *p_dy = p_node->get_value_ref<hmap::HeightMap>("dy");
  hmap::HeightMap *p_ctrl = p_node->get_value_ref<hmap::HeightMap>("control");
  hmap::HeightMap *p_env = p_node->get_value_ref<hmap::HeightMap>("envelope");
  hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("out");

  if (p_ctrl)
  {
    // TODO shape
    hmap::Array ctrl_array = p_ctrl->to_array(hmap::Vec2<int>(128, 128));

    hmap::fill(*p_out,
               p_dx,
               p_dy,
               [p_node, &ctrl_array](hmap::Vec2<int>   shape,
                                     hmap::Vec4<float> bbox,
                                     hmap::Array      *p_noise_x,
                                     hmap::Array      *p_noise_y)
               {
                 return hmap::dendry(shape,
                                     GET("kw", WaveNbAttribute),
                                     GET("seed", SeedAttribute),
                                     ctrl_array,
                                     GET("eps", FloatAttribute),
                                     GET("resolution", IntAttribute),
                                     GET("displacement", FloatAttribute),
                                     GET("primitives_resolution_steps", IntAttribute),
                                     GET("slope_power", FloatAttribute),
                                     GET("noise_amplitude_proportion", FloatAttribute),
                                     true, // add noise
                                     0.5f, // overlap
                                     p_noise_x,
                                     p_noise_y,
                                     nullptr,
                                     bbox,
                                     GET("subsampling", IntAttribute));
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
    post_process_heightmap(*p_out,
                           GET("inverse", BoolAttribute),
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_ATTR("remap_range", RangeAttribute, is_active),
                           GET("remap_range", RangeAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
