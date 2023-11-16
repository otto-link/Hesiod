/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Mask::Mask(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("Mask::Mask()");
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

  if (this->inverse)
    this->value_out.inverse();

  if (this->smoothing)
  {
    hmap::transform(this->value_out,
                    [this](hmap::Array &array)
                    { return hmap::smooth_cpulse(array, this->ir_smoothing); });
    this->value_out.smooth_overlap_buffers();
  }

  if (this->saturate)
  {
    float hmin = this->value_out.min();
    float hmax = this->value_out.max();

    // node parameters are assumed normalized and thus in [0, 1],
    // they need to be rescaled
    float smin_n = hmin + this->smin * (hmax - hmin);
    float smax_n = hmax - (1.f - this->smax) * (hmax - hmin);
    float k_n = this->k * (hmax - hmin);

    hmap::transform(this->value_out,
                    [&smin_n, &smax_n, &k_n](hmap::Array &array)
                    { hmap::clamp_smooth(array, smin_n, smax_n, k_n); });

    // keep original amplitude
    this->value_out.remap(hmin, hmax);
  }

  if (this->remap)
    this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode