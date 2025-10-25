/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/hydrology.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_water_depth_dry_out_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "depth");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "dry_out_ratio", 0.1f, 0.f, 0.5f, "{:.4f}");

  // attribute(s) order
  p_node->set_attr_ordered_key({"dry_out_ratio"});
}

void compute_water_depth_dry_out_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  AppContext &ctx = HSD_CTX;

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("depth");

  if (p_in)
  {
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("water_depth");

    // copy the input heightmap
    *p_out = *p_in;

    float depth_max = p_in->max();

    hmap::transform(
        {p_out, p_mask},
        [p_node, depth_max](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];

          hmap::water_depth_dry_out(*pa_out,
                                    GET("dry_out_ratio", FloatAttribute),
                                    pa_mask,
                                    depth_max);
        },
        ctx.app_settings.node_editor.hmap_transform_mode_cpu);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
