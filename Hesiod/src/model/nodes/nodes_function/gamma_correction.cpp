/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/filters.hpp"
#include "highmap/range.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void setup_gamma_correction_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "gamma", 2.f, 0.01f, 10.f);
}

void compute_gamma_correction_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    float hmin = p_in->min();
    float hmax = p_in->max();

    hmap::transform(
        {p_out, p_in, p_mask},
        [p_node, hmin, hmax](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_mask = p_arrays[2];

          *pa_out = *pa_in;

          hmap::remap(*pa_out, 0.f, 1.f, hmin, hmax);
          hmap::gamma_correction(*pa_out, GET("gamma", FloatAttribute), pa_mask);
          hmap::remap(*pa_out, hmin, hmax, 0.f, 1.f);
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
