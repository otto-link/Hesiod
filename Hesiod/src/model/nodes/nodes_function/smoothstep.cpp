/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/math.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_smoothstep_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  std::vector<std::string> choices = {"3rd", "5th", "7th"};
  ADD_ATTR(ChoiceAttribute, "order", choices);
}

void compute_smoothstep_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    float hmin = p_in->min();
    float hmax = p_in->max();

    hmap::transform(
        {p_out, p_in},
        [p_node, hmin, hmax](std::vector<hmap::Array *> p_arrays,
                             hmap::Vec2<int>,
                             hmap::Vec4<float>)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = *pa_in;

          hmap::remap(*pa_out, 0.f, 1.f, hmin, hmax);

          if (GET("order", ChoiceAttribute) == "3rd")
            *pa_out = hmap::smoothstep3(*pa_out);
          else if (GET("order", ChoiceAttribute) == "5th")
            *pa_out = hmap::smoothstep5(*pa_out);
          else if (GET("order", ChoiceAttribute) == "7th")
            *pa_out = hmap::smoothstep7(*pa_out);

          hmap::remap(*pa_out, hmin, hmax, 0.f, 1.f);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
