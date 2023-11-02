/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

GaussianPulse::GaussianPulse(std::string     id,
                             hmap::Vec2<int> shape,
                             hmap::Vec2<int> tiling,
                             float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("GaussianPulse::GaussianPulse()");
  this->node_type = "GaussianPulse";
  this->category = category_mapping.at(this->node_type);
  this->remove_port("dy");
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void GaussianPulse::compute()
{
  LOG_DEBUG("computing GaussianPulse node [%s]", this->id.c_str());

  std::function<hmap::Array(hmap::Vec2<int>,
                            hmap::Vec2<float>,
                            hmap::Vec2<float>,
                            hmap::Array *)>
      lambda;

  if (this->inverse)
    lambda = [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x)
    {
      return 1.f - hmap::gaussian_pulse(shape,
                                        this->sigma,
                                        p_noise_x,
                                        this->center,
                                        shift,
                                        scale);
    };
  else
    lambda = [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x)
    {
      return hmap::gaussian_pulse(shape,
                                  this->sigma,
                                  p_noise_x,
                                  this->center,
                                  shift,
                                  scale);
    };

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             lambda);

  // remap the output
  this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
