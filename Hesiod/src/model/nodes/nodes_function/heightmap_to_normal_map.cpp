/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/gradient.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_heightmap_to_normal_map_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "normal map", CONFIG(node));

  // attribute(s)

  // attribute(s) order
}

void compute_heightmap_to_normal_map_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::HeightmapRGBA *p_nmap = node.get_value_ref<hmap::HeightmapRGBA>("normal map");

    hmap::Array  array = p_in->to_array();
    hmap::Tensor tn = hmap::normal_map(array);

    hmap::Array nx = tn.get_slice(0);
    hmap::Array ny = tn.get_slice(1);
    hmap::Array nz = tn.get_slice(2);

    *p_nmap = hmap::HeightmapRGBA(p_in->shape,
                                  p_in->tiling,
                                  p_in->overlap,
                                  nx,
                                  ny,
                                  nz,
                                  hmap::Array(p_in->shape, 1.f));
  }
}

} // namespace hesiod
