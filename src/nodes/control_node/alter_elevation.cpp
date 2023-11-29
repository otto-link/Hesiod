/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

AlterElevation::AlterElevation(std::string id) : ControlNode(id)
{
  LOG_DEBUG("AlterElevation::AlterElevation()");
  this->node_type = "AlterElevation";
  this->category = category_mapping.at(this->node_type);

  this->attr["ir"] = NEW_ATTR_INT(64, 1, 256);
  this->attr["footprint_ratio"] = NEW_ATTR_FLOAT(1.f, 0.01f, 4.f);
  this->attr["remap"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"ir", "footprint_ratio", "remap"};

  this->add_port(gnode::Port("cloud", gnode::direction::in, dtype::dCloud));
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));

  this->update_inner_bindings();
}

void AlterElevation::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());
  this->set_p_data("output", (void *)&(this->value_out));
}

void AlterElevation::compute()
{
  LOG_DEBUG("computing AlterElevation node [%s]", this->id.c_str());

  hmap::Cloud     *p_input_cloud = CAST_PORT_REF(hmap::Cloud, "cloud");
  hmap::HeightMap *p_input_hmap = CAST_PORT_REF(hmap::HeightMap, "input");

  this->value_out = *p_input_hmap;
  hmap::transform(this->value_out,
                  [this, p_input_cloud](hmap::Array      &array,
                                        hmap::Vec2<float> shift,
                                        hmap::Vec2<float> scale)
                  {
                    hmap::alter_elevation(array,
                                          *p_input_cloud,
                                          GET_ATTR_INT("ir"),
                                          GET_ATTR_FLOAT("footprint_ratio"),
                                          shift,
                                          scale);
                  });

  this->post_process_heightmap(this->value_out);
}

} // namespace hesiod::cnode
