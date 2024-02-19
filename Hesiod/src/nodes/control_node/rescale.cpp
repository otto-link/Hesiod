/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Rescale::Rescale(std::string id) : ControlNode(id), Unary(id)
{
  this->node_type = "Rescale";
  this->category = category_mapping.at(this->node_type);

  this->attr["scaling"] = NEW_ATTR_FLOAT(1.f, 0.f, 2.f);
  this->attr["centered"] = NEW_ATTR_BOOL(false);

  this->attr_ordered_key = {"scaling", "centered"};
}

void Rescale::compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in)
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  // make a copy of the input and applied range scaleping
  h_out = *p_h_in;

  float vref = 0.f;

  if (GET_ATTR_BOOL("centered"))
    vref = 0.5f * (h_out.min() + h_out.max());

  hmap::transform(h_out,
                  [this, &vref](hmap::Array &x)
                  { hmap::rescale(x, GET_ATTR_FLOAT("scaling"), vref); });
  h_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
