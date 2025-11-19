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
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "erosion", CONFIG);
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG);
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "flow_map", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "c_erosion", 0.1f, 0.01f, 1.f);
  ADD_ATTR(node, FloatAttribute, "talus_ref", 0.1f, 0.01f, 10.f);
  ADD_ATTR(node, FloatAttribute, "deposition_radius", 0.1f, 0.f, 0.2f);
  ADD_ATTR(node, FloatAttribute, "deposition_scale_ratio", 1.f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "gradient_power", 0.8f, 0.1f, 2.f);
  ADD_ATTR(node, FloatAttribute, "gradient_scaling_ratio", 1.f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "gradient_prefilter_radius", 0.1f, 0.f, 0.2f);
  ADD_ATTR(node, FloatAttribute, "saturation_ratio", 1.f, 0.01f, 1.f);
  ADD_ATTR(node, BoolAttribute, "GPU", HSD_DEFAULT_GPU_MODE);

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
  Q_EMIT node.compute_started(node.get_id());

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

    int deposition_ir = (int)(GET(node, "deposition_radius", FloatAttribute) *
                              p_out->shape.x);
    int gradient_ir = (int)(GET(node, "gradient_prefilter_radius", FloatAttribute) *
                            p_out->shape.x);

    deposition_ir = std::max(1, deposition_ir);
    gradient_ir = std::max(1, gradient_ir);

    if (GET(node, "GPU", BoolAttribute))
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
                GET(node, "c_erosion", FloatAttribute),
                GET(node, "talus_ref", FloatAttribute),
                pa_mask,
                deposition_ir,
                GET(node, "deposition_scale_ratio", FloatAttribute),
                GET(node, "gradient_power", FloatAttribute),
                GET(node, "gradient_scaling_ratio", FloatAttribute),
                gradient_ir,
                GET(node, "saturation_ratio", FloatAttribute),
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
                GET(node, "c_erosion", FloatAttribute),
                GET(node, "talus_ref", FloatAttribute),
                pa_mask,
                deposition_ir,
                GET(node, "deposition_scale_ratio", FloatAttribute),
                GET(node, "gradient_power", FloatAttribute),
                GET(node, "gradient_scaling_ratio", FloatAttribute),
                gradient_ir,
                GET(node, "saturation_ratio", FloatAttribute),
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

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
