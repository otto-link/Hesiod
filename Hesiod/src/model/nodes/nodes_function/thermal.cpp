/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "highmap/dbg/timer.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_thermal_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("talus_global", 1.f, 0.f, 16.f, "talus_global");
  p_node->add_attr<IntAttribute>("iterations", 100, 1, 200, "iterations");
  p_node->add_attr<BoolAttribute>("scale_talus_with_elevation",
                                  false,
                                  "scale_talus_with_elevation");
  p_node->add_attr<BoolAttribute>("GPU", true, "GPU");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"talus_global", "iterations", "scale_talus_with_elevation", "_SEPARATOR_", "GPU"});
}

void compute_thermal_node(BaseNode *p_node)
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

    if (GET("scale_talus_with_elevation", BoolAttribute))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 100.f, talus);
    }

    if (GET("GPU", BoolAttribute))
    {
      hmap::Timer::Start("thermal");

      hmap::transform(*p_out,
                      p_mask,
                      &talus_map,
                      p_deposition_map,
                      [p_node, &talus](hmap::Array &h_out,
                                       hmap::Array *p_mask_array,
                                       hmap::Array *p_talus_array,
                                       hmap::Array *p_deposition_array)
                      {
                        hmap::gpu::thermal(h_out,
                                           p_mask_array,
                                           *p_talus_array,
                                           GET("iterations", IntAttribute),
                                           nullptr, // bedrock
                                           p_deposition_array);
                      });

      hmap::Timer::Stop("thermal");
      hmap::Timer::Dump();

      // hmap::Timer::Start("thermal array");

      // hmap::Array out_array = p_in->to_array();
      // hmap::Array talus_array = talus_map.to_array();

      // hmap::gpu::thermal(out_array,
      // 			 nullptr,
      // 			 talus_array,
      // 			 GET("iterations", IntAttribute),
      // 			 nullptr, // bedrock
      // 			 nullptr);

      // p_out->from_array_interp_nearest(out_array);

      // hmap::Timer::Stop("thermal array");
      // hmap::Timer::Dump();
    }
    else
    {
      hmap::transform(*p_out,
                      p_mask,
                      &talus_map,
                      p_deposition_map,
                      [p_node, &talus](hmap::Array &h_out,
                                       hmap::Array *p_mask_array,
                                       hmap::Array *p_talus_array,
                                       hmap::Array *p_deposition_array)
                      {
                        hmap::thermal(h_out,
                                      p_mask_array,
                                      *p_talus_array,
                                      GET("iterations", IntAttribute),
                                      nullptr, // bedrock
                                      p_deposition_array);
                      });
    }

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
