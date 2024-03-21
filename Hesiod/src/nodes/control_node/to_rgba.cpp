/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/cmap.hpp"
#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ToRGBA::ToRGBA(std::string id) : ControlNode(id)
{
  LOG_DEBUG("ToRGBA::ToRGBA()");
  this->node_type = "ToRGBA";
  this->category = category_mapping.at(this->node_type);

  this->add_port(gnode::Port("0 red",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("1 green",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("2 blue",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
  this->add_port(gnode::Port("3 alpha",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));

  this->add_port(
      gnode::Port("RGBA", gnode::direction::out, dtype::dHeightMapRGBA));
  this->update_inner_bindings();
}

void ToRGBA::compute()
{
  LOG_DEBUG("computing ToRGBA node [%s]", this->id.c_str());

  hmap::HeightMap *p_r = CAST_PORT_REF(hmap::HeightMap, "0 red");
  hmap::HeightMap *p_g = CAST_PORT_REF(hmap::HeightMap, "1 green");
  hmap::HeightMap *p_b = CAST_PORT_REF(hmap::HeightMap, "2 blue");
  hmap::HeightMap *p_a = CAST_PORT_REF(hmap::HeightMap, "3 alpha");

  if (p_r || p_g || p_b || p_a)
  {
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

    hmap::HeightMap r = p_r == nullptr
                            ? hmap::HeightMap(shape, tiling, overlap, 0.f)
                            : *p_r;
    hmap::HeightMap g = p_g == nullptr
                            ? hmap::HeightMap(shape, tiling, overlap, 0.f)
                            : *p_g;
    hmap::HeightMap b = p_b == nullptr
                            ? hmap::HeightMap(shape, tiling, overlap, 0.f)
                            : *p_b;
    hmap::HeightMap a = p_a == nullptr
                            ? hmap::HeightMap(shape, tiling, overlap, 1.f)
                            : *p_a;

    this->value_out = hmap::HeightMapRGBA(r, g, b, a);
  }
}

void ToRGBA::update_inner_bindings()
{
  this->set_p_data("RGBA", (void *)&this->value_out);
}

} // namespace hesiod::cnode
