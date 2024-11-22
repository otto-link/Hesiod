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

void setup_recast_cliff_directional_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("angle", 45.f, -180.f, 180.f, "angle");
  p_node->add_attr<FloatAttribute>("talus_global", 1.f, 0.f, 5.f, "talus_global");
  p_node->add_attr<FloatAttribute>("radius", 0.1f, 0.01f, 0.5f, "radius");
  p_node->add_attr<FloatAttribute>("amplitude", 0.1f, 0.f, 1.f, "amplitude");
  p_node->add_attr<FloatAttribute>("gain", 2.f, 0.01f, 10.f, "gain");

  // attribute(s) order
  p_node->set_attr_ordered_key({"angle", "talus_global", "radius", "amplitude", "gain"});
}

void compute_recast_cliff_directional_node(BaseNode *p_node)
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

    float talus = GET("talus_global", FloatAttribute) / (float)p_out->shape.x;
    int   ir = std::max(1, (int)(GET("radius", FloatAttribute) * p_out->shape.x));

    hmap::transform(*p_out,
                    p_mask,
                    [p_node, &talus, &ir](hmap::Array &z, hmap::Array *p_mask)
                    {
                      hmap::recast_cliff_directional(z,
                                                     talus,
                                                     ir,
                                                     GET("amplitude", FloatAttribute),
                                                     GET("angle", FloatAttribute),
                                                     p_mask,
                                                     GET("gain", FloatAttribute));
                    });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
