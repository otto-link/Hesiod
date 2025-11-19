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

void setup_thermal_flatten_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "talus_global", 1.f, 0.f, FLT_MAX);
  ADD_ATTR(node, IntAttribute, "iterations", 100, 1, INT_MAX);
  ADD_ATTR(node, BoolAttribute, "scale_talus_with_elevation", false);
  ADD_ATTR(node, FloatAttribute, "post_filter_radius", 0.01f, 0.f, 0.1f);

  // attribute(s) order
  node.set_attr_ordered_key(
      {"talus_global", "iterations", "scale_talus_with_elevation", "post_filter_radius"});
}

void compute_thermal_flatten_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float talus = GET(node, "talus_global", FloatAttribute) / (float)p_out->shape.x;

    hmap::Heightmap talus_map = hmap::Heightmap(CONFIG, talus);

    if (GET(node, "scale_talus_with_elevation", BoolAttribute))
    {
      talus_map = *p_in;
      talus_map.remap(talus / 100.f, talus);
    }

    int ir = std::max(
        1,
        (int)(GET(node, "post_filter_radius", FloatAttribute) * p_out->shape.x));

    hmap::transform(
        {p_out, &talus_map},
        [&node, talus, ir](std::vector<hmap::Array *> p_arrays,
                           hmap::Vec2<int>            shape,
                           hmap::Vec4<float>)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_talus_map = p_arrays[1];
          hmap::Array  bedrock(shape, -std::numeric_limits<float>::max());

          hmap::thermal_flatten(*pa_out,
                                *pa_talus_map,
                                bedrock,
                                GET(node, "iterations", IntAttribute),
                                ir);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
