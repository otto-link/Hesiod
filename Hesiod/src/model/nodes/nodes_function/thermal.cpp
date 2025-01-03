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
  p_node->add_attr<BoolAttribute>("GPU", HSD_DEFAULT_GPU_MODE, "GPU");

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
      hmap::transform(
          {p_out, p_mask, &talus_map, p_deposition_map},
          [p_node, &talus](std::vector<hmap::Array *> p_arrays,
                           hmap::Vec2<int>,
                           hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];
            hmap::Array *pa_talus_map = p_arrays[2];
            hmap::Array *pa_deposition_map = p_arrays[3];

            hmap::gpu::thermal(*pa_out,
                               pa_mask,
                               *pa_talus_map,
                               GET("iterations", IntAttribute),
                               nullptr, // bedrock
                               pa_deposition_map);
          },
          hmap::TransformMode::DISTRIBUTED);
    }
    else
    {
      hmap::transform(
          {p_out, p_mask, &talus_map, p_deposition_map},
          [p_node, &talus](std::vector<hmap::Array *> p_arrays,
                           hmap::Vec2<int>,
                           hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];
            hmap::Array *pa_talus_map = p_arrays[2];
            hmap::Array *pa_deposition_map = p_arrays[3];

            hmap::thermal(*pa_out,
                          pa_mask,
                          *pa_talus_map,
                          GET("iterations", IntAttribute),
                          nullptr, // bedrock
                          pa_deposition_map);
          },
          hmap::TransformMode::DISTRIBUTED);
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
