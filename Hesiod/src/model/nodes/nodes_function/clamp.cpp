/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_clamp_node(BaseNode *p_node)
{

  LOG->trace("setup_clamp_node");

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<RangeAttribute>("clamp", "clamp");
  p_node->add_attr<BoolAttribute>("smooth_min", false, "smooth_min");
  p_node->add_attr<FloatAttribute>("k_min", 0.05f, 0.01f, 1.f, "k_min");
  p_node->add_attr<BoolAttribute>("smooth_max", false, "smooth_max");
  p_node->add_attr<FloatAttribute>("k_max", 0.05f, 0.01f, 1.f, "k_max");

  // attribute(s) order
  p_node->set_attr_ordered_key({"clamp", "smooth_min", "k_min", "smooth_max", "k_max"});
}

void compute_clamp_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    // retrieve parameters
    hmap::Vec2<float> crange = GET("clamp", RangeAttribute);
    bool              smooth_min = GET("smooth_min", BoolAttribute);
    bool              smooth_max = GET("smooth_max", BoolAttribute);
    float             k_min = GET("k_min", FloatAttribute);
    float             k_max = GET("k_max", FloatAttribute);

    // compute
    if (!smooth_min && !smooth_max)
    {
      hmap::transform(*p_out,
                      [&crange](hmap::Array &x) { hmap::clamp(x, crange.x, crange.y); });
    }
    else
    {
      if (smooth_min)
        hmap::transform(*p_out,
                        [&crange, &k_min](hmap::Array &x)
                        { hmap::clamp_min_smooth(x, crange.x, k_min); });
      else
        hmap::transform(*p_out,
                        [&crange](hmap::Array &x) { hmap::clamp_min(x, crange.x); });

      if (smooth_max)
        hmap::transform(*p_out,
                        [&crange, &k_max](hmap::Array &x)
                        { hmap::clamp_max_smooth(x, crange.y, k_max); });
      else
        hmap::transform(*p_out,
                        [&crange](hmap::Array &x) { hmap::clamp_max(x, crange.y); });
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
