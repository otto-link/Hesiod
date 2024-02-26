/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "macrologger.h"
#include <imgui.h>

#include "hesiod/attribute.hpp"
#include "hesiod/gui.hpp"

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

bool ShapeAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  has_changed |= hesiod::gui::select_shape(label.c_str(),
                                           this->value,
                                           this->value_max);
  return has_changed;
}

bool ShapeAttribute::serialize_json_v2(std::string fieldName, nlohmann::json& outputData) 
{ 
  outputData[fieldName]["value"]["x"] = this->value.x;
  outputData[fieldName]["value"]["y"] = this->value.y;
  outputData[fieldName]["value_max"]["x"] = this->value_max.x;
  outputData[fieldName]["value_max"]["y"] = this->value_max.y;
  return true; 
}

bool ShapeAttribute::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData) 
{ 
  if(
    inputData[fieldName]["value"].is_object() == false ||
    inputData[fieldName]["value_max"].is_object() == false ||
    inputData[fieldName]["value"]["x"].is_number() == false ||
    inputData[fieldName]["value"]["y"].is_number() == false ||
    inputData[fieldName]["value_max"]["x"].is_number() == false ||
    inputData[fieldName]["value_max"]["y"].is_number() == false
  )
  {
    LOG_DEBUG("Attribute %s is not a an array.", fieldName.data());
    return false;
  }

  this->value.x = inputData[fieldName]["value"]["x"].get<float>();
  this->value.y = inputData[fieldName]["value"]["y"].get<float>();
  this->value_max.x = inputData[fieldName]["value_max"]["x"].get<float>();
  this->value_max.y = inputData[fieldName]["value_max"]["y"].get<float>();
  return true; 
}

} // namespace hesiod
