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

  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);
  this->attr["talus_global"] = NEW_ATTR_FLOAT(2.f, 0.01f, 32.f);
  this->attr["center.x"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);
  this->attr["center.y"] = NEW_ATTR_FLOAT(0.5f, -0.5f, 1.5f);

  this->attr_ordered_key = {"angle", "talus_global", "center.x", "center.y"};

  this->remove_port("dy");
  this->update_inner_bindings();
}

void Step::compute()
{
  LOG_DEBUG("computing Step node [%s]", this->id.c_str());

  float talus_global = GET_ATTR_FLOAT("talus_global");
  float talus = talus_global / (float)this->value_out.shape.x;

  hmap::fill(this->value_out,
             (hmap::HeightMap *)this->get_p_data("dx"),
             [this, &talus](hmap::Vec2<int>   shape,
                            hmap::Vec2<float> shift,
                            hmap::Vec2<float> scale,
                            hmap::Array      *p_noise_x)
             {
               hmap::Vec2<float> center;
               center.x = GET_ATTR_FLOAT("center.x");
               center.y = GET_ATTR_FLOAT("center.y");

               return hmap::step(shape,
                                 GET_ATTR_FLOAT("angle"),
                                 talus,
                                 p_noise_x,
                                 center,
                                 shift,
                                 scale);
             });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
