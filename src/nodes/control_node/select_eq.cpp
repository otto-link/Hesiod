/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SelectEq::SelectEq(std::string id) : Unary(id)
{
  this->node_type = "SelectEq";
  this->category = category_mapping.at(this->node_type);
  this->update_inner_bindings();
}

void SelectEq::compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_input)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::transform(h_out,    // output
                  *p_input, // input
                  [this](hmap::Array &array)
                  { return hmap::select_eq(array, this->value); });

  if (this->smoothing)
  {
    hmap::transform(h_out,
                    [this](hmap::Array &array)
                    { return hmap::smooth_cpulse(array, this->ir); });
    h_out.smooth_overlap_buffers();
  }

  if (this->normalize)
    h_out.remap();
}

} // namespace hesiod::cnode
