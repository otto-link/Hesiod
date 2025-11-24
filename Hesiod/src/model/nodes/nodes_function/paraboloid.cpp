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

void setup_paraboloid_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("angle", "angle", 0.f, -180.f, 180.f);
  node.add_attr<FloatAttribute>("a", "a", 1.f, 0.01f, 5.f);
  node.add_attr<FloatAttribute>("b", "b", 1.f, 0.01f, 5.f);
  node.add_attr<FloatAttribute>("v0", "v0", 0.f, -2.f, 2.f);
  node.add_attr<BoolAttribute>("reverse_x", "reverse_x", false);
  node.add_attr<BoolAttribute>("reverse_y", "reverse_y", false);
  node.add_attr<Vec2FloatAttribute>("center", "center");
  node.add_attr<BoolAttribute>("inverse", "inverse", false);
  node.add_attr<RangeAttribute>("remap", "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"angle",
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

void compute_paraboloid_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  hmap::transform(
      {p_out, p_dx, p_dy},
      [&node](std::vector<hmap::Array *> p_arrays,
              hmap::Vec2<int>            shape,
              hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_dx = p_arrays[1];
        hmap::Array *pa_dy = p_arrays[2];

        *pa_out = hmap::paraboloid(shape,
                                   node.get_attr<FloatAttribute>("angle"),
                                   node.get_attr<FloatAttribute>("a"),
                                   node.get_attr<FloatAttribute>("b"),
                                   node.get_attr<FloatAttribute>("v0"),
                                   node.get_attr<BoolAttribute>("reverse_x"),
                                   node.get_attr<BoolAttribute>("reverse_y"),
                                   pa_dx,
                                   pa_dy,
                                   nullptr,
                                   node.get_attr<Vec2FloatAttribute>("center"),
                                   bbox);
      },
      node.get_config_ref()->hmap_transform_mode_cpu);

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);

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

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
