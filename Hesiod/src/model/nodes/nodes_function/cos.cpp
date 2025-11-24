/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/math.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_cos_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("frequency", "frequency", 1.f, 0.f, FLT_MAX);
  node.add_attr<FloatAttribute>("phase_shift", "phase_shift", 0.f, 0.f, 6.28f);
  node.add_attr<BoolAttribute>("inverse", "inverse", false);
  node.add_attr<RangeAttribute>("remap", "remap");

  // attribute(s) order
  node.set_attr_ordered_key(
      {"frequency", "phase_shift", "_SEPARATOR_", "inverse", "remap"});
}

void compute_cos_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    hmap::transform({p_out, p_in},
                    [&node](std::vector<hmap::Array *> p_arrays)
                    {
                      hmap::Array *pa_out = p_arrays[0];
                      hmap::Array *pa_in = p_arrays[1];

                      *pa_out = hmap::cos(6.283185f *
                                              node.get_attr<FloatAttribute>("frequency") *
                                              (*pa_in) +
                                          node.get_attr<FloatAttribute>("phase_shift"));
                    });

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
}

} // namespace hesiod
