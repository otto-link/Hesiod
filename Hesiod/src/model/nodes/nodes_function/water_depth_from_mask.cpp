/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/hydrology.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_water_depth_from_mask_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "water_mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "mask_threshold", 0.01f, 0.f, 0.1f, "{:.3f}");
  ADD_ATTR(node, FloatAttribute, "tolerance", 1e-5f, 1e-6f, 1e-2f, "{:.3e}", true);
  ADD_ATTR(node, IntAttribute, "iterations", 500, 1, INT_MAX);
  ADD_ATTR(node, FloatAttribute, "omega", 1.8f, 1e-3f, 1.9f);

  // attribute(s) order
  node.set_attr_ordered_key({"mask_threshold", "tolerance", "iterations", "omega"});

  add_wip_warning_label(node);
}

void compute_water_depth_from_mask_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_z = node.get_value_ref<hmap::Heightmap>("elevation");
  hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("water_mask");

  if (p_z && p_mask)
  {
    hmap::Heightmap *p_depth = node.get_value_ref<hmap::Heightmap>("water_depth");

    hmap::transform(
        {p_depth, p_z, p_mask},
        [&node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_depth = p_arrays[0];
          hmap::Array *pa_z = p_arrays[1];
          hmap::Array *pa_mask = p_arrays[2];

          *pa_depth = hmap::water_depth_from_mask(
              *pa_z,
              *pa_mask,
              GET(node, "mask_threshold", FloatAttribute),
              GET(node, "iterations", IntAttribute),
              GET(node, "tolerance", FloatAttribute),
              GET(node, "omega", FloatAttribute));
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    p_depth->smooth_overlap_buffers();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
