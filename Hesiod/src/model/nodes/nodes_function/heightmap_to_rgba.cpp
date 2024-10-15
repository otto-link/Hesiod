/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_heightmap_to_rgba_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "R");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "G");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "B");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "A");
  p_node->add_port<hmap::HeightMapRGBA>(gnode::PortType::OUT, "RGBA", CONFIG);
}

void compute_heightmap_to_rgba_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_r = p_node->get_value_ref<hmap::HeightMap>("R");
  hmap::HeightMap *p_g = p_node->get_value_ref<hmap::HeightMap>("G");
  hmap::HeightMap *p_b = p_node->get_value_ref<hmap::HeightMap>("B");
  hmap::HeightMap *p_a = p_node->get_value_ref<hmap::HeightMap>("A");

  if (p_r || p_g || p_b || p_a)
  {
    hmap::HeightMapRGBA *p_out = p_node->get_value_ref<hmap::HeightMapRGBA>("RGBA");

    hmap::Vec2<int> shape;
    hmap::Vec2<int> tiling;
    float           overlap = 0.f;

    for (auto ptr : {p_r, p_g, p_b, p_a})
      if (ptr)
      {
        shape = ptr->shape;
        tiling = ptr->tiling;
        overlap = ptr->overlap;
        break;
      }

    hmap::HeightMap hr = p_r == nullptr ? hmap::HeightMap(shape, tiling, overlap, 0.f)
                                        : *p_r;
    hmap::HeightMap hg = p_g == nullptr ? hmap::HeightMap(shape, tiling, overlap, 0.f)
                                        : *p_g;
    hmap::HeightMap hb = p_b == nullptr ? hmap::HeightMap(shape, tiling, overlap, 0.f)
                                        : *p_b;
    hmap::HeightMap ha = p_a == nullptr ? hmap::HeightMap(shape, tiling, overlap, 1.f)
                                        : *p_a;

    *p_out = hmap::HeightMapRGBA(hr, hg, hb, ha);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
