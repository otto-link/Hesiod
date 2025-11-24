/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/gradient.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_normal_map_to_heightmap_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "normal map");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<BoolAttribute>("poisson_solver", "poisson_solver", false);
  node.add_attr<IntAttribute>("iterations", "iterations", 500, 1, INT_MAX);
  node.add_attr<FloatAttribute>("omega", "omega", 1.5f, 1e-3f, 2.f);

  // attribute(s) order
  node.set_attr_ordered_key({"poisson_solver", "iterations", "omega"});

  setup_post_process_heightmap_attributes(node);
}

void compute_normal_map_to_heightmap_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::HeightmapRGBA *p_nmap = node.get_value_ref<hmap::HeightmapRGBA>("normal map");

  if (p_nmap)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    hmap::Tensor ts = hmap::Tensor(p_nmap->shape, 3);
    ts.set_slice(0, p_nmap->rgba[0].to_array());
    ts.set_slice(1, p_nmap->rgba[1].to_array());
    ts.set_slice(2, p_nmap->rgba[2].to_array());

    hmap::Array z;

    if (node.get_attr<BoolAttribute>("poisson_solver"))
    {
      z = hmap::normal_map_to_heightmap_poisson(ts,
                                                node.get_attr<IntAttribute>("iterations"),
                                                node.get_attr<FloatAttribute>("omega"));
    }
    else
    {
      z = hmap::normal_map_to_heightmap(ts);
    }

    hmap::remap(z);

    p_out->from_array_interp(z);

    // post-process
    post_process_heightmap(node, *p_out);
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
