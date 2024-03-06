/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "macrologger.h"

#include "hesiod/attribute.hpp"

namespace hesiod
{

ShapeAttribute::ShapeAttribute()
{
}

hmap::Vec2<int> ShapeAttribute::get()
{
  return value;
}

void ShapeAttribute::set_value_max(hmap::Vec2<int> new_value_max)
{
  this->value_max = new_value_max;
}

bool ShapeAttribute::serialize_json_v2(std::string     field_name,
                                       nlohmann::json &output_data)
{
  output_data[field_name]["value"]["x"] = this->value.x;
  output_data[field_name]["value"]["y"] = this->value.y;
  output_data[field_name]["value_max"]["x"] = this->value_max.x;
  output_data[field_name]["value_max"]["y"] = this->value_max.y;
  return true;
}

bool ShapeAttribute::deserialize_json_v2(std::string     field_name,
                                         nlohmann::json &input_data)
{
  if (input_data[field_name]["value"].is_object() == false ||
      input_data[field_name]["value_max"].is_object() == false ||
      input_data[field_name]["value"]["x"].is_number() == false ||
      input_data[field_name]["value"]["y"].is_number() == false ||
      input_data[field_name]["value_max"]["x"].is_number() == false ||
      input_data[field_name]["value_max"]["y"].is_number() == false)
  {
    LOG_DEBUG("Attribute %s is not a an array.", field_name.data());
    return false;
  }

  this->value.x = input_data[field_name]["value"]["x"].get<float>();
  this->value.y = input_data[field_name]["value"]["y"].get<float>();
  this->value_max.x = input_data[field_name]["value_max"]["x"].get<float>();
  this->value_max.y = input_data[field_name]["value_max"]["y"].get<float>();
  return true;
}

} // namespace hesiod
