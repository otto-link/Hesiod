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

void setup_normal_map_to_heightmap_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "normal map");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(IntAttribute, "iterations", 500, 1, INT_MAX);
  ADD_ATTR(FloatAttribute, "omega", 1.5f, 1e-3f, 2.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"iterations", "omega"});

  setup_post_process_heightmap_attributes(p_node);

  add_wip_warning_label(p_node);
}

void compute_normal_map_to_heightmap_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::HeightmapRGBA *p_nmap = p_node->get_value_ref<hmap::HeightmapRGBA>("normal map");

  if (p_nmap)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    hmap::Tensor ts = hmap::Tensor(p_nmap->shape, 3);
    ts.set_slice(0, p_nmap->rgba[0].to_array());
    ts.set_slice(1, p_nmap->rgba[1].to_array());
    ts.set_slice(2, p_nmap->rgba[2].to_array());

    hmap::Array z = hmap::normal_map_to_heightmap_poisson(ts,
                                                          GET("iterations", IntAttribute),
                                                          GET("omega", FloatAttribute));
    hmap::remap(z);

    p_out->from_array_interp(z);

    // post-process
    post_process_heightmap(p_node, *p_out);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
