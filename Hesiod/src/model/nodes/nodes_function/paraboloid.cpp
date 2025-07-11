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

void setup_paraboloid_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "angle", 0.f, -180.f, 180.f);
  ADD_ATTR(FloatAttribute, "a", 1.f, 0.01f, 5.f);
  ADD_ATTR(FloatAttribute, "b", 1.f, 0.01f, 5.f);
  ADD_ATTR(FloatAttribute, "v0", 0.f, -2.f, 2.f);
  ADD_ATTR(BoolAttribute, "reverse_x", false);
  ADD_ATTR(BoolAttribute, "reverse_y", false);
  ADD_ATTR(Vec2FloatAttribute, "center");
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(RangeAttribute, "remap");

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
                                "remap"});
}

void compute_paraboloid_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // base noise function
  hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_env = p_node->get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

  hmap::transform(
      {p_out, p_dx, p_dy},
      [p_node](std::vector<hmap::Array *> p_arrays,
               hmap::Vec2<int>            shape,
               hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_dx = p_arrays[1];
        hmap::Array *pa_dy = p_arrays[2];

        *pa_out = hmap::paraboloid(shape,
                                   GET("angle", FloatAttribute),
                                   GET("a", FloatAttribute),
                                   GET("b", FloatAttribute),
                                   GET("v0", FloatAttribute),
                                   GET("reverse_x", BoolAttribute),
                                   GET("reverse_y", BoolAttribute),
                                   pa_dx,
                                   pa_dy,
                                   nullptr,
                                   GET("center", Vec2FloatAttribute),
                                   bbox);
      },
      p_node->get_config_ref()->hmap_transform_mode_cpu);

  // post-process
  post_apply_enveloppe(p_node, *p_out, p_env);

  post_process_heightmap(p_node,
                         *p_out,
                         GET("inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_MEMBER("remap", RangeAttribute, is_active),
                         GET("remap", RangeAttribute));

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
