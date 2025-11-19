/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_depression_filling_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "fill map", CONFIG(node));

  // attribute(s)
  node.add_attr<IntAttribute>("iterations", "iterations", 1000, 1, INT_MAX);

  node.add_attr<FloatAttribute>("epsilon", "epsilon", 1e-4, 1e-5, 1e-1, "{:.3e}", true);

  node.add_attr<BoolAttribute>("remap fill map", "remap fill map", true);

  // attribute(s) order
  node.set_attr_ordered_key({"iterations", "epsilon", "remap fill map"});
}

void compute_depression_filling_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_fill_map = node.get_value_ref<hmap::Heightmap>("fill map");

    float epsilon_normalized = node.get_attr<FloatAttribute>("epsilon") /
                               (float)p_in->shape.x;

    hmap::transform(
        {p_out, p_in},
        [&node, epsilon_normalized](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = *pa_in;

          hmap::depression_filling(*pa_out,
                                   node.get_attr<IntAttribute>("iterations"),
                                   epsilon_normalized);
        },
        hmap::TransformMode::SINGLE_ARRAY); // forced, not tileable

    hmap::transform(*p_fill_map,
                    *p_in,
                    *p_out,
                    [&node](hmap::Array &out, hmap::Array &in1, hmap::Array &in2)
                    { out = in2 - in1; });

    if (node.get_attr<BoolAttribute>("remap fill map"))
      p_fill_map->remap();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
