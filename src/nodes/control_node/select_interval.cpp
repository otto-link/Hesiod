/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SelectInterval::SelectInterval(std::string id) : Mask(id)
{
  this->node_type = "SelectInterval";
  this->category = category_mapping.at(this->node_type);
  this->update_inner_bindings();
}

void SelectInterval::compute_mask(hmap::HeightMap &h_out,
                                  hmap::HeightMap *p_input)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::transform(
      h_out,    // output
      *p_input, // input
      [this](hmap::Array &array)
      { return hmap::select_interval(array, this->value1, this->value2); });
}

} // namespace hesiod::cnode
