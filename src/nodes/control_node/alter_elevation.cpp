/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

AlterElevation::AlterElevation(std::string id) : gnode::Node(id)
{
  LOG_DEBUG("AlterElevation::AlterElevation()");
  this->node_type = "AlterElevation";
  this->category = category_mapping.at(this->node_type);

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

  hmap::Cloud *p_input_cloud = static_cast<hmap::Cloud *>(
      this->get_p_data("cloud"));
  hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
      this->get_p_data("input"));

  this->value_out = *p_input_hmap;
  hmap::transform(this->value_out,
                  [this, p_input_cloud](hmap::Array      &array,
                                        hmap::Vec2<float> shift,
                                        hmap::Vec2<float> scale)
                  {
                    hmap::alter_elevation(array,
                                          *p_input_cloud,
                                          this->ir,
                                          this->footprint_ratio,
                                          shift,
                                          scale);
                  });

  // remap the output
  this->value_out.remap(this->vmin, this->vmax);
}

} // namespace hesiod::cnode
