/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Import::Import(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap)
    : ControlNode(id), shape(shape), tiling(tiling), overlap(overlap)
{
  LOG_DEBUG("Import::Import()");
  this->node_type = "Import";
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->category = category_mapping.at(this->node_type);
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void Import::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

void Import::compute()
{
  // check if the file exist before loading
  std::ifstream f(this->fname.c_str());
  if (f.good())
  {
    hmap::Array z = hmap::Array(this->fname);
    this->value_out.from_array_interp(z);
    this->value_out.remap(this->vmin, this->vmax);
  }
}

} // namespace hesiod::cnode
