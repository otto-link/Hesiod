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

void setup_crater_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "control");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("radius", 0.1f, 0.01f, 1.f, "radius");
  p_node->add_attr<FloatAttribute>("depth", 0.2f, 0.f, 1.f, "depth");
  p_node->add_attr<FloatAttribute>("lip_decay", 0.1f, 0.01f, 1.f, "lip_decay");
  p_node->add_attr<FloatAttribute>("lip_height_ratio",
                                   2.f,
                                   0.01f,
                                   10.f,
                                   "lip_height_ratio");
  p_node->add_attr<Vec2FloatAttribute>("center", "center");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<RangeAttribute>("remap_range", "remap_range");

  // attribute(s) order
  p_node->set_attr_ordered_key({"radius",
                                "depth",
                                "lip_decay",
                                "lip_height_ratio",
                                "center",
                                "_SEPARATOR_",
                                "inverse",
                                "remap_range"});
}

void compute_crater_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::HeightMap *p_dx = p_node->get_value_ref<hmap::HeightMap>("dx");
  hmap::HeightMap *p_dy = p_node->get_value_ref<hmap::HeightMap>("dy");
  hmap::HeightMap *p_ctrl = p_node->get_value_ref<hmap::HeightMap>("control");
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
               return hmap::crater(shape,
                                   GET("radius", FloatAttribute),
                                   GET("depth", FloatAttribute),
                                   GET("lip_decay", FloatAttribute),
                                   GET("lip_height_ratio", FloatAttribute),
                                   p_ctrl,
                                   p_noise_x,
                                   p_noise_y,
                                   GET("center", Vec2FloatAttribute),
                                   bbox);
             });

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