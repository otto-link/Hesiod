/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/math.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_abs_smooth_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("mu", "mu", 0.05f, 0.001f, 0.4f);
  node.add_attr<FloatAttribute>("vshift", "vshift", 0.5f, 0.f, 1.f);
  node.add_attr<BoolAttribute>("inverse", "inverse", false);
  node.add_attr<RangeAttribute>("remap", "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"mu", "vshift", "_SEPARATOR_", "inverse", "remap"});
}

void compute_abs_smooth_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    hmap::transform(
        {p_out, p_in},
        [&node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          *pa_out = hmap::abs_smooth(*pa_in,
                                     node.get_attr<FloatAttribute>("mu"),
                                     node.get_attr<FloatAttribute>("vshift"));
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

    // post-process
    post_process_heightmap(node,
                           *p_out,
                           node.get_attr<BoolAttribute>("inverse"),
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           node.get_attr_ref<RangeAttribute>("remap")->get_is_active(),
                           node.get_attr<RangeAttribute>("remap"));
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
