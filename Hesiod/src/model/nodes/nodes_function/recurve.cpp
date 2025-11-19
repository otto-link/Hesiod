/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/operator.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_recurve_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  std::vector<float> default_values = {0.f, 0.25f, 0.5f, 0.75f, 1.f};

  ADD_ATTR(node, VecFloatAttribute, "values", default_values, 0.f, 1.f);

  add_wip_warning_label(node);
}

void compute_recurve_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    if (GET(node, "values", VecFloatAttribute).size() >= 3)
    {
      float hmin = p_in->min();
      float hmax = p_in->max();

      std::vector<float> t = hmap::linspace(
          0.f,
          1.f,
          GET(node, "values", VecFloatAttribute).size());

      hmap::transform(
          {p_out, p_in, p_mask},
          [&node, t, hmin, hmax](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];
            hmap::Array *pa_mask = p_arrays[2];

            *pa_out = *pa_in;

            hmap::remap(*pa_out, 0.f, 1.f, hmin, hmax);
            hmap::recurve(*pa_out, t, GET(node, "values", VecFloatAttribute), pa_mask);
            hmap::remap(*pa_out, hmin, hmax, 0.f, 1.f);
          },
          node.get_config_ref()->hmap_transform_mode_cpu);
    }
    else
    {
      // don't do anything if not enough values
      Logger::log()->warn("not enough values, at least 4 points required");
    }
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
