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

void setup_flooding_lake_system_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG(node));

  // attribute(s)
  node.add_attr<IntAttribute>("iterations", "iterations", 1000, 1, INT_MAX);

  node.add_attr<FloatAttribute>("epsilon", "epsilon", 1e-1, 1e-5, 1e-1, "{:.3e}", true);

  node.add_attr<FloatAttribute>("mininal_radius", "mininal_radius", 0.05f, 0.f, 0.5f);

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Main Parameters",
                             "iterations",
                             "epsilon",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Exclusion filter",
                             "mininal_radius",
                             "_GROUPBOX_END_"});
}

void compute_flooding_lake_system_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("elevation");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("water_depth");

    float epsilon_normalized = node.get_attr<FloatAttribute>("epsilon") /
                               (float)p_in->shape.x;
    int   ir = node.get_attr<FloatAttribute>("mininal_radius") * (float)p_in->shape.x;
    float surface_threshold = M_PI * ir * ir;

    hmap::transform(
        {p_out, p_in},
        [&node, epsilon_normalized, surface_threshold](
            std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = hmap::flooding_lake_system(*pa_in,
                                               node.get_attr<IntAttribute>("iterations"),
                                               epsilon_normalized,
                                               surface_threshold);
        },
        hmap::TransformMode::SINGLE_ARRAY); // forced, not tileable
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
