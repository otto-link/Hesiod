/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

GradientAngle::GradientAngle(std::string id) : ControlNode(id), Unary(id)
{
  this->node_type = "GradientAngle";
  this->category = category_mapping.at(this->node_type);
  this->update_inner_bindings();
}

void GradientAngle::compute_in_out(hmap::HeightMap &angle,
                                   hmap::HeightMap *p_input)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::transform(angle,    // output
                  *p_input, // input
                  [](hmap::Array &z) { return hmap::gradient_angle(z); });
  angle.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
