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
  this->category = category_mapping.at(this->node_type);

  this->attr["fname"] = NEW_ATTR_FILENAME("");
  this->attr["remap"] = NEW_ATTR_RANGE(true);

  this->attr_ordered_key = {"fname", "remap"};

  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));

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
  std::ifstream f(GET_ATTR_FILENAME("fname").c_str());
  if (f.good())
  {
    hmap::Array z = hmap::Array(GET_ATTR_FILENAME("fname"));
    this->value_out.from_array_interp(z);
    this->post_process_heightmap(this->value_out);
  }
}

} // namespace hesiod::cnode
