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

void setup_sediment_deposition_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("talus_global", 0.2f, 0.f, 4.f, "talus_global");
  p_node->add_attr<FloatAttribute>("max_deposition", 0.001f, 0.f, 0.1f, "max_deposition");
  p_node->add_attr<IntAttribute>("iterations", 1, 1, 20, "iterations");
  p_node->add_attr<IntAttribute>("thermal_subiterations",
                                 50,
                                 1,
                                 200,
                                 "thermal_subiterations");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"talus_global", "max_deposition", "iterations", "thermal_subiterations"});
}

void compute_sediment_deposition_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_deposition_map = p_node->get_value_ref<hmap::Heightmap>(
        "deposition");

    // copy the input heightmap
    *p_out = *p_in;

    float talus = GET("talus_global", FloatAttribute) / (float)p_out->shape.x;

    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG, talus);

    // if (GET("scale_talus_with_elevation", BoolAttribute))
    // {
    //   talus_map = *p_in;
    //   talus_map.remap(talus / 100.f, talus);
    // }

    // void sediment_deposition(Array       &z,
    //                        Array       *p_mask,
    //                        const Array &talus,
    //                        Array       *p_deposition_map = nullptr,
    //                        float        max_deposition = 0.01,
    //                        int          iterations = 5,
    //                        int          thermal_subiterations = 10);

    hmap::transform(*p_out,
                    p_mask,
                    &talus_map,
                    p_deposition_map,
                    [p_node, &talus](hmap::Array &h_out,
                                     hmap::Array *p_mask_array,
                                     hmap::Array *p_talus_array,
                                     hmap::Array *p_deposition_array)
                    {
                      hmap::sediment_deposition(
                          h_out,
                          p_mask_array,
                          *p_talus_array,
                          p_deposition_array,
                          GET("max_deposition", FloatAttribute),
                          GET("iterations", IntAttribute),
                          GET("thermal_subiterations", IntAttribute));
                    });

    p_out->smooth_overlap_buffers();

    if (p_deposition_map)
    {
      p_deposition_map->smooth_overlap_buffers();
      p_deposition_map->remap();
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
