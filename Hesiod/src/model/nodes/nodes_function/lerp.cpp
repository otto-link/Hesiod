/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/math.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_lerp_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "a");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "b");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "t");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "t", 0.5f, 0.f, 1.f);
}

void compute_lerp_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_a = node.get_value_ref<hmap::Heightmap>("a");
  hmap::Heightmap *p_b = node.get_value_ref<hmap::Heightmap>("b");

  if (p_a && p_b)
  {
    hmap::Heightmap *p_t = node.get_value_ref<hmap::Heightmap>("t");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    if (p_t)
      hmap::transform(
          {p_out, p_a, p_b, p_t},
          [](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_a = p_arrays[1];
            hmap::Array *pa_b = p_arrays[2];
            hmap::Array *pa_t = p_arrays[3];

            *pa_out = hmap::lerp(*pa_a, *pa_b, *pa_t);
          },
          node.get_config_ref()->hmap_transform_mode_cpu);
    else
      hmap::transform(
          {p_out, p_a, p_b},
          [&node](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_a = p_arrays[1];
            hmap::Array *pa_b = p_arrays[2];

            *pa_out = hmap::lerp(*pa_a, *pa_b, GET(node, "t", FloatAttribute));
          },
          node.get_config_ref()->hmap_transform_mode_cpu);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
