/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/filters.hpp"
#include "highmap/heightmap.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_reverse_above_theshold_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "threshold");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "threshold_value", 0.5f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(FloatAttribute, "scaling", 0.5f, 0.f, 2.f);
  ADD_ATTR(FloatAttribute, "transition_extent", 0.1f, 0.f, 1.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"threshold_value", "scaling", "transition_extent"});

  setup_post_process_heightmap_attributes(p_node, true);
}

void compute_reverse_above_theshold_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_th = p_node->get_value_ref<hmap::Heightmap>("threshold");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    hmap::transform(
        {p_out, p_in, p_th, p_mask},
        [p_node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];
          hmap::Array *pa_th = p_arrays[2];
          hmap::Array *pa_mask = p_arrays[3];

          *pa_out = *pa_in;

          if (pa_th)
          {
            hmap::reverse_above_theshold(*pa_out,
                                         *pa_th,
                                         pa_mask,
                                         GET("scaling", FloatAttribute),
                                         GET("transition_extent", FloatAttribute));
          }
          else
          {
            hmap::reverse_above_theshold(*pa_out,
                                         GET("threshold_value", FloatAttribute),
                                         pa_mask,
                                         GET("scaling", FloatAttribute),
                                         GET("transition_extent", FloatAttribute));
          }
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);

    // post-process
    post_process_heightmap(p_node, *p_out, p_in);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
