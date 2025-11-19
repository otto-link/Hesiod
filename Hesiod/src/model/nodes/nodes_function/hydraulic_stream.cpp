/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_stream_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "erosion", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "c_erosion", 0.05f, 0.01f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "talus_ref", 0.1f, 0.01f, 10.f);
  ADD_ATTR(node, FloatAttribute, "radius", 0.f, 0.f, 0.05f);
  ADD_ATTR(node, FloatAttribute, "clipping_ratio", 10.f, 0.1f, 100.f);

  // attribute(s) order
  node.set_attr_ordered_key({"c_erosion", "talus_ref", "radius", "clipping_ratio"});
}

void compute_hydraulic_stream_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_erosion_map = node.get_value_ref<hmap::Heightmap>("erosion");

    // copy the input heightmap
    *p_out = *p_in;

    int ir = (int)(GET(node, "radius", FloatAttribute) * p_out->shape.x);

    hmap::transform(
        {p_out, p_mask, p_erosion_map},
        [&node, &ir](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];
          hmap::Array *pa_erosion_map = p_arrays[2];

          hmap::hydraulic_stream(*pa_out,
                                 pa_mask,
                                 GET(node, "c_erosion", FloatAttribute),
                                 GET(node, "talus_ref", FloatAttribute),
                                 nullptr,
                                 nullptr,
                                 pa_erosion_map,
                                 ir,
                                 GET(node, "clipping_ratio", FloatAttribute));
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    p_out->smooth_overlap_buffers();

    p_erosion_map->smooth_overlap_buffers();
    p_erosion_map->remap();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
