/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_thermal_schott_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("talus_global", 1.f, 0.f, 4.f, "talus_global");
  // p_node->add_attr<FloatAttribute>("intensity", 0.0001f, 0.f, 0.1f, "intensity");
  p_node->add_attr<IntAttribute>("iterations", 10, 1, 200, "iterations");
  p_node->add_attr<BoolAttribute>("scale_talus_with_elevation",
                                  false,
                                  "scale_talus_with_elevation");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"talus_global", "iterations", "scale_talus_with_elevation"});
}

void compute_thermal_schott_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float talus = GET("talus_global", FloatAttribute) / (float)p_out->shape.x;
    float intensity = 0.05f * talus;

    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG, talus);

    if (GET("scale_talus_with_elevation", BoolAttribute))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 100.f, talus);
    }

    hmap::transform(*p_out,
                    &talus_map,
                    [p_node, intensity](hmap::Array &h_out, hmap::Array *p_talus_array)
                    {
                      hmap::thermal_schott(h_out,
                                           *p_talus_array,
                                           GET("iterations", IntAttribute),
                                           intensity);
                    });

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
