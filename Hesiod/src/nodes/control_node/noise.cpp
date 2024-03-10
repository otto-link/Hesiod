/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Noise::Noise(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap)
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("Noise::Noise()");
  this->node_type = "Noise";
  this->category = category_mapping.at(this->node_type);

  this->attr["noise_type"] = NEW_ATTR_MAPENUM(this->noise_type_map);
  this->attr["kw"] = NEW_ATTR_WAVENB();
  this->attr["seed"] = NEW_ATTR_SEED();

  this->attr_ordered_key = {"noise_type", "kw", "seed"};

  this->add_port(gnode::Port("envelope",
                             gnode::direction::in,
                             dtype::dHeightMap,
                             gnode::optional::yes));
}

void Noise::compute()
{
  LOG_DEBUG("computing Noise node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec4<float> bbox,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::noise(
                   (hmap::NoiseType)GET_ATTR_MAPENUM("noise_type"),
                   shape,
                   GET_ATTR_WAVENB("kw"),
                   GET_ATTR_SEED("seed"),
                   p_noise_x,
                   p_noise_y,
                   nullptr,
                   bbox);
             });

  if (this->get_p_data("envelope"))
  {
    float hmin = this->value_out.min();
    LOG_DEBUG("hmin: %f", hmin);
    hmap::transform(this->value_out,
                    *(hmap::HeightMap *)this->get_p_data("envelope"),
                    [&hmin](hmap::Array &a, hmap::Array &b)
                    {
                      a -= hmin;
                      a *= b;
                    });
  }

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
