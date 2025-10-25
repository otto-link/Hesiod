/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_heightmap_to_rgba_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "R");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "G");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "B");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "A");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "RGBA", CONFIG);
}

void compute_heightmap_to_rgba_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_r = p_node->get_value_ref<hmap::Heightmap>("R");
  hmap::Heightmap *p_g = p_node->get_value_ref<hmap::Heightmap>("G");
  hmap::Heightmap *p_b = p_node->get_value_ref<hmap::Heightmap>("B");
  hmap::Heightmap *p_a = p_node->get_value_ref<hmap::Heightmap>("A");

  if (p_r || p_g || p_b || p_a)
  {
    hmap::HeightmapRGBA *p_out = p_node->get_value_ref<hmap::HeightmapRGBA>("RGBA");

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

    hmap::Heightmap hr = p_r == nullptr ? hmap::Heightmap(shape, tiling, overlap, 0.f)
                                        : *p_r;
    hmap::Heightmap hg = p_g == nullptr ? hmap::Heightmap(shape, tiling, overlap, 0.f)
                                        : *p_g;
    hmap::Heightmap hb = p_b == nullptr ? hmap::Heightmap(shape, tiling, overlap, 0.f)
                                        : *p_b;
    hmap::Heightmap ha = p_a == nullptr ? hmap::Heightmap(shape, tiling, overlap, 1.f)
                                        : *p_a;

    *p_out = hmap::HeightmapRGBA(hr, hg, hb, ha);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
