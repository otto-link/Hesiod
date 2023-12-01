/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Mask::Mask(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Mask::Mask()");

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing"] = NEW_ATTR_BOOL(false);
  this->attr["ir_smoothing"] = NEW_ATTR_INT(4, 1, 128);
  this->attr["saturate"] = NEW_ATTR_RANGE(false);
  this->attr["k_saturate"] = NEW_ATTR_FLOAT(0.05f, 0.f, 1.f);
  this->attr["remap"] = NEW_ATTR_RANGE(true);

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void Mask::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void Mask::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));

  this->value_out.set_sto(p_input->shape, p_input->tiling, p_input->overlap);

  this->compute_mask(this->value_out, p_input);
  this->value_out.smooth_overlap_buffers();
  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
