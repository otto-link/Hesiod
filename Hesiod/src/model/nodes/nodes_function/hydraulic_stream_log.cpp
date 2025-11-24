/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_stream_log_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "erosion", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "flow_map", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("c_erosion", "c_erosion", 0.1f, 0.01f, 1.f);
  node.add_attr<FloatAttribute>("talus_ref", "talus_ref", 0.1f, 0.01f, 10.f);
  node.add_attr<FloatAttribute>("deposition_radius",
                                "deposition_radius",
                                0.1f,
                                0.f,
                                0.2f);
  node.add_attr<FloatAttribute>("deposition_scale_ratio",
                                "deposition_scale_ratio",
                                1.f,
                                0.f,
                                1.f);
  node.add_attr<FloatAttribute>("gradient_power", "gradient_power", 0.8f, 0.1f, 2.f);
  node.add_attr<FloatAttribute>("gradient_scaling_ratio",
                                "gradient_scaling_ratio",
                                1.f,
                                0.f,
                                1.f);
  node.add_attr<FloatAttribute>("gradient_prefilter_radius",
                                "gradient_prefilter_radius",
                                0.1f,
                                0.f,
                                0.2f);
  node.add_attr<FloatAttribute>("saturation_ratio", "saturation_ratio", 1.f, 0.01f, 1.f);
  node.add_attr<BoolAttribute>("GPU", "GPU", HSD_DEFAULT_GPU_MODE);

  // attribute(s) order
  node.set_attr_ordered_key({"c_erosion",
                             "deposition_radius",
                             "deposition_scale_ratio",
                             "gradient_power",
                             "gradient_scaling_ratio",
                             "gradient_prefilter_radius",
                             "_TEXT_Some text",
                             "saturation_ratio",
                             "talus_ref",
                             // "_SEPARATOR_",
                             "GPU"});

  setup_post_process_heightmap_attributes(node, true);
}

void compute_hydraulic_stream_log_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_erosion_map = node.get_value_ref<hmap::Heightmap>("erosion");
    hmap::Heightmap *p_deposition_map = node.get_value_ref<hmap::Heightmap>("deposition");
    hmap::Heightmap *p_flow_map = node.get_value_ref<hmap::Heightmap>("flow_map");

    // copy the input heightmap
    *p_out = *p_in;

    int deposition_ir = (int)(node.get_attr<FloatAttribute>("deposition_radius") *
                              p_out->shape.x);
    int gradient_ir = (int)(node.get_attr<FloatAttribute>("gradient_prefilter_radius") *
                            p_out->shape.x);

    deposition_ir = std::max(1, deposition_ir);
    gradient_ir = std::max(1, gradient_ir);

    if (node.get_attr<BoolAttribute>("GPU"))
    {
      hmap::transform(
          {p_out, p_mask, p_erosion_map, p_deposition_map, p_flow_map},
          [&node, deposition_ir, gradient_ir](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];
            hmap::Array *pa_erosion_map = p_arrays[2];
            hmap::Array *pa_deposition_map = p_arrays[3];
            hmap::Array *pa_flow_map = p_arrays[4];

            hmap::gpu::hydraulic_stream_log(
                *pa_out,
                node.get_attr<FloatAttribute>("c_erosion"),
                node.get_attr<FloatAttribute>("talus_ref"),
                pa_mask,
                deposition_ir,
                node.get_attr<FloatAttribute>("deposition_scale_ratio"),
                node.get_attr<FloatAttribute>("gradient_power"),
                node.get_attr<FloatAttribute>("gradient_scaling_ratio"),
                gradient_ir,
                node.get_attr<FloatAttribute>("saturation_ratio"),
                nullptr,
                nullptr,
                pa_erosion_map,
                pa_deposition_map,
                pa_flow_map);
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      hmap::transform(
          {p_out, p_mask, p_erosion_map, p_deposition_map, p_flow_map},
          [&node, deposition_ir, gradient_ir](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_mask = p_arrays[1];
            hmap::Array *pa_erosion_map = p_arrays[2];
            hmap::Array *pa_deposition_map = p_arrays[3];
            hmap::Array *pa_flow_map = p_arrays[4];

            hmap::hydraulic_stream_log(
                *pa_out,
                node.get_attr<FloatAttribute>("c_erosion"),
                node.get_attr<FloatAttribute>("talus_ref"),
                pa_mask,
                deposition_ir,
                node.get_attr<FloatAttribute>("deposition_scale_ratio"),
                node.get_attr<FloatAttribute>("gradient_power"),
                node.get_attr<FloatAttribute>("gradient_scaling_ratio"),
                gradient_ir,
                node.get_attr<FloatAttribute>("saturation_ratio"),
                nullptr,
                nullptr,
                pa_erosion_map,
                pa_deposition_map,
                pa_flow_map);
          },
          node.get_config_ref()->hmap_transform_mode_cpu);
    }

    p_out->smooth_overlap_buffers();

    p_erosion_map->smooth_overlap_buffers();
    p_erosion_map->remap();

    p_deposition_map->smooth_overlap_buffers();
    p_deposition_map->remap();

    p_flow_map->smooth_overlap_buffers();
    p_flow_map->remap();

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
