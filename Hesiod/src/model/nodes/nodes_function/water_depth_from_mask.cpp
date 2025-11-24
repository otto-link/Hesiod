/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/hydrology.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
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
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("mask_threshold",
                                "mask_threshold",
                                0.01f,
                                0.f,
                                0.1f,
                                "{:.3f}");
  node.add_attr<FloatAttribute>("tolerance",
                                "tolerance",
                                1e-5f,
                                1e-6f,
                                1e-2f,
                                "{:.3e}",
                                true);
  node.add_attr<IntAttribute>("iterations", "iterations", 500, 1, INT_MAX);
  node.add_attr<FloatAttribute>("omega", "omega", 1.8f, 1e-3f, 1.9f);

  // attribute(s) order
  node.set_attr_ordered_key({"mask_threshold", "tolerance", "iterations", "omega"});
}

void compute_water_depth_from_mask_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

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
              node.get_attr<FloatAttribute>("mask_threshold"),
              node.get_attr<IntAttribute>("iterations"),
              node.get_attr<FloatAttribute>("tolerance"),
              node.get_attr<FloatAttribute>("omega"));
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    p_depth->smooth_overlap_buffers();
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
