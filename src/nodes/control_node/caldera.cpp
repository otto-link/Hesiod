/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Caldera::Caldera(std::string     id,
                 hmap::Vec2<int> shape,
                 hmap::Vec2<int> tiling,
                 float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("Caldera::Caldera()");
  this->node_type = "Caldera";
  this->category = category_mapping.at(this->node_type);
  this->remove_port("dy");
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void Caldera::compute()
{
  LOG_DEBUG("computing Caldera node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x)
             {
               return hmap::caldera(shape,
                                    this->radius,
                                    this->sigma_inner,
                                    this->sigma_outer,
                                    this->z_bottom,
                                    p_noise_x,
                                    this->noise_r_amp,
                                    this->noise_ratio_z,
                                    this->center,
                                    shift,
                                    scale);
             });

  this->value_out.smooth_overlap_buffers();
  this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
