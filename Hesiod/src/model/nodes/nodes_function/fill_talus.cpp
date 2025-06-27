/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_fill_talus_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "slope", 4.f, 0.1f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "noise_ratio", 0.2f, 0.f, 1.f);
  ADD_ATTR(SeedAttribute, "seed");

  // attribute(s) order
  p_node->set_attr_ordered_key({"slope", "noise_ratio", "seed"});
}

void compute_fill_talus_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    float talus = GET("slope", FloatAttribute) / (float)p_out->shape.x;

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out,
                    [p_node, &talus](hmap::Array &x)
                    {
                      hmap::fill_talus(x,
                                       talus,
                                       GET("seed", SeedAttribute),
                                       GET("noise_ratio", FloatAttribute));
                    });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
