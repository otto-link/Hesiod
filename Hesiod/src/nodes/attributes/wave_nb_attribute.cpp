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

WaveNbAttribute::WaveNbAttribute(hmap::Vec2<float> value) : value(value)
{
}

hmap::Vec2<float> WaveNbAttribute::get()
{
  return value;
}

bool WaveNbAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  has_changed |= hesiod::gui::drag_float_vec2(this->value, this->link_xy);
  return has_changed;
}

bool WaveNbAttribute::serialize_json_v2(std::string fieldName, nlohmann::json& outputData) 
{ 
  outputData[fieldName]["value"]["x"] = this->value.x;
  outputData[fieldName]["value"]["y"] = this->value.y;
  outputData[fieldName]["link_xy"] = this->link_xy;

  return true; 
}

bool WaveNbAttribute::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData) 
{ 
  if(
    inputData[fieldName].is_object() == false ||
    inputData[fieldName]["value"].is_object() == false ||
    inputData[fieldName]["link_xy"].is_boolean() == false ||
    inputData[fieldName]["value"]["x"].is_number() == false ||
    inputData[fieldName]["value"]["y"].is_number() == false
  )
  {
    LOG_DEBUG("Attribute %s is not a an array.", fieldName.data());
    return false;
  }

  this->value.x = inputData[fieldName]["value"]["x"].get<float>();
  this->value.y = inputData[fieldName]["value"]["y"].get<float>();
  this->link_xy = inputData[fieldName]["link_xy"].get<bool>();
  return true; 
}

} // namespace hesiod
