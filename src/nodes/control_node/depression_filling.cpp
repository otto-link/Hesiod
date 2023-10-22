/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

DepressionFilling::DepressionFilling(std::string id) : Unary(id)
{
  LOG_DEBUG("DepressionFilling::DepressionFilling()");
  this->node_type = "DepressionFilling";
  this->category = category_mapping.at(this->node_type);
}

void DepressionFilling::compute_in_out(hmap::HeightMap &h_out,
                                       hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  h_out = *p_h_in; // copy the input

  {
    // hmap::transform(
    // 		    h_out,
    // 		    [this](hmap::Array &x)
    // 		    { hmap::depression_filling(x, this->iterations,
    // this->epsilon);
    // });
  }

  // --- work on a single array as a temporary solution
  hmap::Array z_array = h_out.to_array();
  hmap::depression_filling(z_array, this->iterations, this->epsilon);
  h_out.from_array_interp(z_array);
}

} // namespace hesiod::cnode
