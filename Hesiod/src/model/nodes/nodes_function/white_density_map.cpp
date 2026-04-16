/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_white_density_map_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "density");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<SeedAttribute>("seed", "Seed");
  // attribute(s) order
  node.set_attr_ordered_key({"seed"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = true});
}

void compute_white_density_map_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::VirtualArray *p_density = node.get_value_ref<hmap::VirtualArray>("density");

  if (p_density)
  {
    hmap::VirtualArray *p_env = node.get_value_ref<hmap::VirtualArray>("envelope");
    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");

    // base noise function
    int seed = node.get_attr<SeedAttribute>("seed");

    hmap::for_each_tile(
        {p_out, p_density},
        [&seed](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_density = p_arrays[1];

          *pa_out = hmap::white_density_map(*pa_density, (uint)seed++);
        },
        node.cfg().cm_cpu);

    // post-process
    post_apply_enveloppe(node, *p_out, p_env);
    post_process_heightmap(node, *p_out);
  }
}

} // namespace hesiod
