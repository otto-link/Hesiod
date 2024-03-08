/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/array.hpp"
#include "highmap/heightmap.hpp"
#include "macrologger.h"

#include "hesiod/control_node.hpp"
#include "hesiod/serialization.hpp"

namespace hesiod::cnode
{

Brush::Brush(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap)
    : ControlNode(id), shape(shape), tiling(tiling), overlap(overlap)
{
  LOG_DEBUG("Brush::Brush()");
  this->node_type = "Brush";
  this->category = category_mapping.at(this->node_type);

  this->attr["remap"] = NEW_ATTR_RANGE(true);

  this->add_port(
      gnode::Port("output", gnode::direction::out, dtype::dHeightMap));
  this->value_out.set_sto(shape, tiling, overlap);
  this->update_inner_bindings();
}

void Brush::update_inner_bindings()
{
  this->set_p_data("output", (void *)&this->value_out);
}

void Brush::compute()
{
  LOG_DEBUG("computing Brush node [%s]", this->id.c_str());

  this->post_process_heightmap(this->value_out);
}

bool Brush::serialize_json_v2(std::string     field_name,
                              nlohmann::json &output_data)
{
  if (ControlNode::serialize_json_v2(field_name, output_data) == false)
  {
    return false;
  }

  output_data[field_name]["heightmap"]["array"] =
      serialization::adapter_hmap_array_serialize(this->value_out.to_array());
  output_data[field_name]["heightmap"]["shape"] =
      serialization::adapter_hmap_serialize_vec2<int>(this->value_out.shape);
  return true;
}

bool Brush::deserialize_json_v2(std::string     field_name,
                                nlohmann::json &input_data)
{
  if (input_data[field_name].is_object() == false ||
      input_data[field_name]["heightmap"].is_object() == false)
  {
    return false;
  }

  if (ControlNode::deserialize_json_v2(field_name, input_data) == false)
  {
    return false;
  }

  hmap::Array array = serialization::adapter_hmap_array_deserialize(
      input_data[field_name]["heightmap"]["array"]);

  this->value_out.from_array_interp(array);

  return true;
}

} // namespace hesiod::cnode
