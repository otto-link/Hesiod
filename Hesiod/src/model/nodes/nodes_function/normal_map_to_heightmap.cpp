/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/gradient.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_normal_map_to_heightmap_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "normal map");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, BoolAttribute, "poisson_solver", false);
  ADD_ATTR(node, IntAttribute, "iterations", 500, 1, INT_MAX);
  ADD_ATTR(node, FloatAttribute, "omega", 1.5f, 1e-3f, 2.f);

  // attribute(s) order
  node.set_attr_ordered_key({"poisson_solver", "iterations", "omega"});

  setup_post_process_heightmap_attributes(node);

  add_wip_warning_label(node);
}

void compute_normal_map_to_heightmap_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

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

    if (GET(node, "poisson_solver", BoolAttribute))
    {
      z = hmap::normal_map_to_heightmap_poisson(ts,
                                                GET(node, "iterations", IntAttribute),
                                                GET(node, "omega", FloatAttribute));
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

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
