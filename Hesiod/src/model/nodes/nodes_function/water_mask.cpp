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

void setup_water_mask_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "water_depth");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "mask", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "additional_depth", 0.f, 0.f, 0.2f);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"_GROUPBOX_BEGIN_Main parameters", "additional_depth", "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(p_node);
}

void compute_water_mask_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_z = p_node->get_value_ref<hmap::Heightmap>("elevation");
  hmap::Heightmap *p_depth = p_node->get_value_ref<hmap::Heightmap>("water_depth");

  if (p_z && p_depth)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");

    hmap::transform(
        {p_depth, p_z, p_mask},
        [p_node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_depth = p_arrays[0];
          hmap::Array *pa_z = p_arrays[1];
          hmap::Array *pa_mask = p_arrays[2];

          float added_depth = GET("additional_depth", FloatAttribute);

          if (added_depth)
          {
            *pa_mask = hmap::water_mask(*pa_depth, *pa_z, added_depth);
          }
          else
          {
            *pa_mask = hmap::water_mask(*pa_depth);
          }
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    // post-process
    p_mask->smooth_overlap_buffers();
    post_process_heightmap(p_node, *p_mask);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
