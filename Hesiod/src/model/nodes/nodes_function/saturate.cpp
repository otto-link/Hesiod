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

void setup_saturate_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<RangeAttribute>("range", "range");
  p_node->add_attr<FloatAttribute>("k_smoothing", 0.1f, 0.01, 1.f, "k_smoothing");
}

void compute_saturate_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float hmin = p_out->min();
    float hmax = p_out->max();

    hmap::transform(*p_out,
                    [p_node, &hmin, &hmax](hmap::Array &x)
                    {
                      hmap::saturate(x,
                                     GET("range", RangeAttribute)[0],
                                     GET("range", RangeAttribute)[1],
                                     hmin,
                                     hmax,
                                     GET("k_smoothing", FloatAttribute));
                    });
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod