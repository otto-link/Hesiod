/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SelectTransitions::SelectTransitions(std::string id) : gnode::Node(id)
{
  this->node_type = "SelectTransitions";
  this->category = category_mapping.at(this->node_type);
  this->add_port(
      gnode::Port("input##1", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("input##2", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("blend", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->update_inner_bindings();
}

void SelectTransitions::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void SelectTransitions::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  hmap::HeightMap *p_input_hmap1 = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input##1"));
  hmap::HeightMap *p_input_hmap2 = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("input##2"));
  hmap::HeightMap *p_input_blend = static_cast<hmap::HeightMap *>(
      (void *)this->get_p_data("blend"));

  this->value_out.set_sto(p_input_hmap1->shape,
                          p_input_hmap1->tiling,
                          p_input_hmap1->overlap);

  hmap::transform(
      this->value_out,
      *p_input_hmap1,
      *p_input_hmap2,
      *p_input_blend,
      [](hmap::Array &m, hmap::Array &a1, hmap::Array &a2, hmap::Array &a3)
      { m = hmap::select_transitions(a1, a2, a3); });

  if (this->smoothing)
  {
    hmap::transform(this->value_out,
                    [this](hmap::Array &array)
                    { return hmap::smooth_cpulse(array, this->ir); });
    this->value_out.smooth_overlap_buffers();
  }
}

} // namespace hesiod::cnode
