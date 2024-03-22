/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Bump::Bump(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap)
    : ControlNode(id), Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("Bump::Bump()");
  this->node_type = "Bump";
  this->category = category_mapping.at(this->node_type);

  this->attr["gain"] = NEW_ATTR_FLOAT(1.f, 0.01f, 10.f);
}

void Bump::compute()
{
  LOG_DEBUG("computing Bump node [%s]", this->id.c_str());

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             (hmap::HeightMap *)this->get_p_data("dy"),
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec4<float> bbox,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y)
             {
               return hmap::bump(shape,
                                 GET_ATTR_FLOAT("gain"),
                                 p_noise_x,
                                 p_noise_y,
                                 nullptr,
                                 bbox);
             });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
