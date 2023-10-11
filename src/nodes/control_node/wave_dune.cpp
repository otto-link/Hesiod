/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

WaveDune::WaveDune(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("WaveDune::WaveDune()");
  this->node_type = "WaveDune";
  this->category = category_mapping.at(this->node_type);
  this->remove_port("dy");
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void WaveDune::compute()
{
  LOG_DEBUG("computing WaveDune node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec2<float> shift,
                    hmap::Vec2<float> scale,
                    hmap::Array      *p_noise_x)
             {
               return hmap::wave_dune(shape,
                                      this->kw,
                                      this->angle,
                                      this->xtop,
                                      this->xbottom,
                                      this->phase_shift,
                                      p_noise_x,
                                      shift,
                                      scale);
             });

  // remap the output
  this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
