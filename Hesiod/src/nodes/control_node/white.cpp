/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

White::White(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap)
    : ControlNode(id), shape(shape), tiling(tiling), overlap(overlap)
{
  LOG_DEBUG("White::WhiteFilter()");
  this->node_type = "White";
  this->category = category_mapping.at(this->node_type);

  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["remap"] = NEW_ATTR_RANGE(false);
  this->attr_ordered_key = {"seed", "remap"};

  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void White::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void White::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  int seed = (int)GET_ATTR_SEED("seed");

  hmap::fill(this->value_out,
             [&seed](hmap::Vec2<int> shape)
             { return hmap::white(shape, 0.f, 1.f, (uint)seed++); });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
