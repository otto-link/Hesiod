/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_recast_cracks_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("cut_min", "cut_min", 0.05f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("cut_max", "cut_max", 0.5f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("k_smoothing", "k_smoothing", 0.01f, 0.f, 1.f);
}

void compute_recast_cracks_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float hmin = p_in->min();
    float hmax = p_in->max();

    hmap::transform(
        {p_out},
        [&node, hmin, hmax](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];

          hmap::recast_cracks(*pa_out,
                              node.get_attr<FloatAttribute>("cut_min"),
                              node.get_attr<FloatAttribute>("cut_max"),
                              node.get_attr<FloatAttribute>("k_smoothing"),
                              hmin,
                              hmax);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);

    p_out->smooth_overlap_buffers();
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
