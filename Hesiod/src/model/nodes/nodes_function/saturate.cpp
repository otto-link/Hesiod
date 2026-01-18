/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
#include "highmap/operator.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_saturate_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("k_smoothing", "k_smoothing", 0.1f, 0.01, 1.f);
  node.add_attr<RangeAttribute>("range", "range");

  // link histogram for RangeAttribute
  setup_histogram_for_range_attribute(node, "range", "input");

  // attribute(s) order
  node.set_attr_ordered_key({"k_smoothing", "range"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = false});
}

void compute_saturate_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  if (p_in)
  {
    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");

    float hmin = p_in->min(node.cfg().cm_cpu);
    float hmax = p_in->max(node.cfg().cm_cpu);

    hmap::for_each_tile(
        {p_out, p_in},
        [&node, &hmin, &hmax](std::vector<hmap::Array *> p_arrays,
                              const hmap::TileRegion &)
        {
          auto [pa_out, pa_in] = unpack<2>(p_arrays);
          *pa_out = *pa_in;

          hmap::saturate(*pa_out,
                         node.get_attr<RangeAttribute>("range")[0],
                         node.get_attr<RangeAttribute>("range")[1],
                         hmin,
                         hmax,
                         node.get_attr<FloatAttribute>("k_smoothing"));
        },
        node.cfg().cm_cpu);

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }
}

} // namespace hesiod
