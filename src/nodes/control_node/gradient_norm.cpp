/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

GradientNorm::GradientNorm(std::string id) : ControlNode(id), Unary(id)
{
  this->node_type = "GradientNorm";
  this->category = category_mapping.at(this->node_type);

  this->attr["remap"] = NEW_ATTR_RANGE();

  this->update_inner_bindings();
}

void GradientNorm::compute_in_out(hmap::HeightMap &gradient_norm,
                                  hmap::HeightMap *p_input)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::transform(gradient_norm, // output
                  *p_input,      // input
                  [](hmap::Array &z) { return hmap::gradient_norm(z); });

  gradient_norm.smooth_overlap_buffers();
  this->post_process_heightmap(gradient_norm);
}

} // namespace hesiod::cnode
