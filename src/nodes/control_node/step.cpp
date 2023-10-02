/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Step::Step(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap)
    : Primitive(id, shape, tiling, overlap)
{
  LOG_DEBUG("Step::Step()");
  this->node_type = "Step";
  this->category = category_mapping.at(this->node_type);
  this->remove_port("dy");
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void Step::compute()
{
  LOG_DEBUG("computing Step node [%s]", this->id.c_str());

  float talus = this->talus_global / (float)this->value_out.shape.x;

  hmap::fill(
      this->value_out,
      (hmap::HeightMap *)this->get_p_data("dx"),
      [this, &talus](hmap::Vec2<int>   shape,
                     hmap::Vec2<float> shift,
                     hmap::Vec2<float> scale,
                     hmap::Array      *p_noise_x) {
        return hmap::step(shape, this->angle, talus, p_noise_x, shift, scale);
      });

  // remap the output
  this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
