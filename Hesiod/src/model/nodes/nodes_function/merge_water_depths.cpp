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

void setup_merge_water_depths_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "depth1");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "depth2");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "k_smooth", 0.f, 0.f, 0.1f, "{:.4f}");

  // attribute(s) order
  p_node->set_attr_ordered_key({"k_smooth"});
}

void compute_merge_water_depths_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in1 = p_node->get_value_ref<hmap::Heightmap>("depth1");
  hmap::Heightmap *p_in2 = p_node->get_value_ref<hmap::Heightmap>("depth2");

  if (p_in1 && p_in2)
  {
    hmap::Heightmap *p_depth = p_node->get_value_ref<hmap::Heightmap>("water_depth");

    hmap::transform(
        {p_depth, p_in1, p_in2},
        [p_node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_depth = p_arrays[0];
          hmap::Array *pa_in1 = p_arrays[1];
          hmap::Array *pa_in2 = p_arrays[2];

          *pa_depth = hmap::merge_water_depths(*pa_in1,
                                               *pa_in2,
                                               GET("k_smooth", FloatAttribute));
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    p_depth->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
