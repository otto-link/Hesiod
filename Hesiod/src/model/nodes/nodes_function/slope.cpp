/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_slope_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "angle", 0.f, -180.f, 180.f);
  ADD_ATTR(node, FloatAttribute, "talus_global", 2.f, 0.01f, FLT_MAX);
  ADD_ATTR(node, Vec2FloatAttribute, "center");
  ADD_ATTR(node, BoolAttribute, "inverse", false);
  ADD_ATTR(node, RangeAttribute, "remap");

  // attribute(s) order
  node.set_attr_ordered_key(
      {"angle", "talus_global", "center", "_SEPARATOR_", "inverse", "remap"});
}

void compute_slope_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = node.get_value_ref<hmap::Heightmap>("control");
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
               return hmap::slope(shape,
                                  GET(node, "angle", FloatAttribute),
                                  GET(node, "talus_global", FloatAttribute),
                                  p_ctrl,
                                  p_noise_x,
                                  p_noise_y,
                                  nullptr,
                                  GET(node, "center", Vec2FloatAttribute),
                                  bbox);
             });

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
