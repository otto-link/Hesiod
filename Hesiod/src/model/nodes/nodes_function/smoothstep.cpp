/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/math.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_smoothstep_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  std::vector<std::string> choices = {"3rd", "5th", "7th"};
  node.add_attr<ChoiceAttribute>("order", "order", choices);
}

void compute_smoothstep_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    float hmin = p_in->min();
    float hmax = p_in->max();

    hmap::transform(
        {p_out, p_in},
        [&node, hmin, hmax](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = *pa_in;

          hmap::remap(*pa_out, 0.f, 1.f, hmin, hmax);

          if (node.get_attr<ChoiceAttribute>("order") == "3rd")
            *pa_out = hmap::smoothstep3(*pa_out);
          else if (node.get_attr<ChoiceAttribute>("order") == "5th")
            *pa_out = hmap::smoothstep5(*pa_out);
          else if (node.get_attr<ChoiceAttribute>("order") == "7th")
            *pa_out = hmap::smoothstep7(*pa_out);

          hmap::remap(*pa_out, hmin, hmax, 0.f, 1.f);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);
  }
}

} // namespace hesiod
