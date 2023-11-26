/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ConvolveSVD::ConvolveSVD(std::string id) : ControlNode(id)
{
  this->node_type = "ConvolveSVD";
  this->category = category_mapping.at(this->node_type);
  this->attr["rank"] = NEW_ATTR_INT(4, 1, 8);

  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("kernel", gnode::direction::in, dtype::dArray));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void ConvolveSVD::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void ConvolveSVD::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());

  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input"));
  hmap::Array *p_input_kernel = static_cast<hmap::Array *>(
      (void *)this->get_p_data("kernel"));

  this->value_out = *p_input_hmap;

  hmap::transform(
      this->value_out,
      [this, p_input_kernel](hmap::Array &z)
      { z = hmap::convolve2d_svd(z, *p_input_kernel, GET_ATTR_INT("rank")); });

  this->value_out.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
