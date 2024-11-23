/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/operator.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_recurve_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  std::vector<float> default_values = {0.f, 0.25f, 0.5f, 0.75f, 1.f};

  p_node->add_attr<VecFloatAttribute>("values", default_values, 0.f, 1.f, "values");
}

void compute_recurve_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    if (GET("values", VecFloatAttribute).size() >= 3)
    {
      float hmin = p_out->min();
      float hmax = p_out->max();

      p_out->remap(0.f, 1.f, hmin, hmax);

      std::vector<float> t = hmap::linspace(0.f,
                                            1.f,
                                            GET("values", VecFloatAttribute).size());

      hmap::transform(*p_out,
                      p_mask,
                      [p_node, t](hmap::Array &x, hmap::Array *p_mask)
                      { hmap::recurve(x, t, GET("values", VecFloatAttribute), p_mask); });

      p_out->remap(hmin, hmax, 0.f, 1.f);
    }
    else
    {
      // don't do anything if not enough values
      LOG->warn("not enough values, at least 4 points required");
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
