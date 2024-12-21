/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_particle_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "bedrock");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "moisture");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "erosion", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG);

  // attribute(s)
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<FloatAttribute>("particle_density",
                                   0.2f,
                                   0.f,
                                   1.f,
                                   "particle_density");
  p_node->add_attr<FloatAttribute>("c_capacity", 10.f, 0.1f, 100.f, "c_capacity");
  p_node->add_attr<FloatAttribute>("c_erosion", 0.05f, 0.01f, 0.1f, "c_erosion");
  p_node->add_attr<FloatAttribute>("c_deposition", 0.05f, 0.01f, 0.1f, "c_deposition");
  p_node->add_attr<FloatAttribute>("drag_rate", 0.001f, 0.f, 1.f, "drag_rate");
  p_node->add_attr<FloatAttribute>("evap_rate", 0.001f, 0.f, 1.f, "evap_rate");
  p_node->add_attr<BoolAttribute>("GPU", true, "GPU");

  // attribute(s) order
  p_node->set_attr_ordered_key({"seed",
                                "particle_density",
                                "c_capacity",
                                "c_erosion",
                                "c_deposition",
                                "drag_rate",
                                "evap_rate",
                                "_SEPARATOR_",
                                "GPU"});
}

void compute_hydraulic_particle_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_bedrock = p_node->get_value_ref<hmap::Heightmap>("bedrock");
    hmap::Heightmap *p_moisture_map = p_node->get_value_ref<hmap::Heightmap>("moisture");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");

    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_erosion_map = p_node->get_value_ref<hmap::Heightmap>("erosion");
    hmap::Heightmap *p_deposition_map = p_node->get_value_ref<hmap::Heightmap>(
        "deposition");

    // copy the input heightmap
    *p_out = *p_in;

    int nparticles = (int)(GET("particle_density", FloatAttribute) * p_out->shape.x *
                           p_out->shape.y);

    int nparticles_tile = (int)(nparticles / (float)p_out->get_ntiles());

    if (GET("GPU", BoolAttribute))
    {
      hmap::transform(*p_out,
                      p_bedrock,
                      p_moisture_map,
                      p_mask,
                      p_erosion_map,
                      p_deposition_map,
                      [p_node, &nparticles_tile](hmap::Array &h_out,
                                                 hmap::Array *p_bedrock_array,
                                                 hmap::Array *p_moisture_map_array,
                                                 hmap::Array *p_mask_array,
                                                 hmap::Array *p_erosion_map_array,
                                                 hmap::Array *p_deposition_map_array)
                      {
                        hmap::hydraulic_particle(h_out,
                                                 p_mask_array,
                                                 nparticles_tile,
                                                 GET("seed", SeedAttribute),
                                                 p_bedrock_array,
                                                 p_moisture_map_array,
                                                 p_erosion_map_array,
                                                 p_deposition_map_array,
                                                 GET("c_capacity", FloatAttribute),
                                                 GET("c_erosion", FloatAttribute),
                                                 GET("c_deposition", FloatAttribute),
                                                 GET("drag_rate", FloatAttribute),
                                                 GET("evap_rate", FloatAttribute));
                      });
    }
    else
    {
      hmap::transform(*p_out,
                      p_bedrock,
                      p_moisture_map,
                      p_mask,
                      p_erosion_map,
                      p_deposition_map,
                      [p_node, &nparticles_tile](hmap::Array &h_out,
                                                 hmap::Array *p_bedrock_array,
                                                 hmap::Array *p_moisture_map_array,
                                                 hmap::Array *p_mask_array,
                                                 hmap::Array *p_erosion_map_array,
                                                 hmap::Array *p_deposition_map_array)
                      {
                        hmap::gpu::hydraulic_particle(h_out,
                                                      p_mask_array,
                                                      nparticles_tile,
                                                      GET("seed", SeedAttribute),
                                                      p_bedrock_array,
                                                      p_moisture_map_array,
                                                      p_erosion_map_array,
                                                      p_deposition_map_array,
                                                      GET("c_capacity", FloatAttribute),
                                                      GET("c_erosion", FloatAttribute),
                                                      GET("c_deposition", FloatAttribute),
                                                      GET("drag_rate", FloatAttribute),
                                                      GET("evap_rate", FloatAttribute));
                      });
    }

    p_out->smooth_overlap_buffers();

    if (p_erosion_map)
    {
      p_erosion_map->smooth_overlap_buffers();
      p_erosion_map->remap();
    }

    if (p_deposition_map)
    {
      p_deposition_map->smooth_overlap_buffers();
      p_deposition_map->remap();
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
