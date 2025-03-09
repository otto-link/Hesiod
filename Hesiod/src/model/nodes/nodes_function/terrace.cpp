/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_terrace_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(IntAttribute, "nlevels", 4, 1, 32);
  ADD_ATTR(FloatAttribute, "gain", 2.f, 0.01f, 10.f);
  ADD_ATTR(FloatAttribute, "noise_ratio", 0.1f, 0.f, 0.5f);
  ADD_ATTR(SeedAttribute, "seed");

  // attribute(s) order
  p_node->set_attr_ordered_key({"nlevels", "gain", "noise_ratio", "seed"});
}

void compute_terrace_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_noise = p_node->get_value_ref<hmap::Heightmap>("noise");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float hmin = p_out->min();
    float hmax = p_out->max();

    hmap::transform(
        *p_out,
        p_noise,
        p_mask,
        [p_node, hmin, hmax](hmap::Array &x, hmap::Array *p_noise, hmap::Array *p_mask)
        {
          hmap::terrace(x,
                        GET("seed", SeedAttribute),
                        GET("nlevels", IntAttribute),
                        p_mask,
                        GET("gain", FloatAttribute),
                        GET("noise_ratio", FloatAttribute),
                        p_noise,
                        hmin,
                        hmax);
        });
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
