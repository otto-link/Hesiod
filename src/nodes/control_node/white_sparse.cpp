/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

WhiteSparse::WhiteSparse(std::string     id,
                         hmap::Vec2<int> shape,
                         hmap::Vec2<int> tiling,
                         float           overlap)
    : gnode::Node(id), shape(shape), tiling(tiling), overlap(overlap)
{
  LOG_DEBUG("WhiteSparse::WhiteSparseFilter()");

  this->node_type = "WhiteSparse";
  this->category = category_mapping.at(this->node_type);
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void WhiteSparse::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void WhiteSparse::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  float density_per_tile = this->density / (float)this->value_out.get_ntiles();
  int   seed0 = this->seed;

  hmap::fill(this->value_out,
             [&density_per_tile, &seed0](hmap::Vec2<int> shape)
             {
               return hmap::white_sparse(shape,
                                         0.f,
                                         1.f,
                                         density_per_tile,
                                         (uint)seed0++);
             });

  // remap the output
  this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
