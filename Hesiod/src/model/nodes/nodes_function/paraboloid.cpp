/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_paraboloid_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("angle", 0.f, -180.f, 180.f, "angle");
  p_node->add_attr<FloatAttribute>("a", 1.f, 0.01f, 5.f, "a");
  p_node->add_attr<FloatAttribute>("b", 1.f, 0.01f, 5.f, "b");
  p_node->add_attr<FloatAttribute>("v0", 0.f, -2.f, 2.f, "v0");
  p_node->add_attr<BoolAttribute>("reverse_x", false, "reverse_x");
  p_node->add_attr<BoolAttribute>("reverse_y", false, "reverse_y");
  p_node->add_attr<Vec2FloatAttribute>("center", "center");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");
  p_node->add_attr<RangeAttribute>("remap_range", "remap_range");

  // attribute(s) order
  p_node->set_attr_ordered_key({"angle",
                                "a",
                                "b",
                                "v0",
                                "reverse_x",
                                "reverse_y",
                                "center",
                                "_SEPARATOR_",
                                "inverse",
                                "remap_range"});
}

void compute_paraboloid_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::HeightMap *p_dx = p_node->get_value_ref<hmap::HeightMap>("dx");
  hmap::HeightMap *p_dy = p_node->get_value_ref<hmap::HeightMap>("dy");
  hmap::HeightMap *p_env = p_node->get_value_ref<hmap::HeightMap>("envelope");
  hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

  hmap::fill(*p_out,
             p_dx,
             p_dy,
             [p_node](hmap::Vec2<int>   shape,
                      hmap::Vec4<float> bbox,
                      hmap::Array      *p_noise_x,
                      hmap::Array      *p_noise_y)
             {
               return hmap::paraboloid(shape,
                                       GET("angle", FloatAttribute),
                                       GET("a", FloatAttribute),
                                       GET("b", FloatAttribute),
                                       GET("v0", FloatAttribute),
                                       GET("reverse_x", BoolAttribute),
                                       GET("reverse_y", BoolAttribute),
                                       p_noise_x,
                                       p_noise_y,
                                       nullptr,
                                       GET("center", Vec2FloatAttribute),
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
