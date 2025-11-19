/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_rescale_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "scaling", 1.f, 0.0001f, FLT_MAX, "{:.4f}");
  ADD_ATTR(node, BoolAttribute, "centered", false);

  // attribute(s) order
  node.set_attr_ordered_key({"scaling", "centered"});
}

void compute_rescale_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float vref = 0.f;

    if (GET(node, "centered", BoolAttribute))
      vref = p_out->mean();

    hmap::transform(
        {p_out},
        [&node, vref](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];

          hmap::rescale(*pa_out, GET(node, "scaling", FloatAttribute), vref);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
