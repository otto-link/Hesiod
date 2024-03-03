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

bool WaveNbAttribute::serialize_json_v2(std::string     field_name,
                                        nlohmann::json &output_data)
{
  output_data[field_name]["value"]["x"] = this->value.x;
  output_data[field_name]["value"]["y"] = this->value.y;
  output_data[field_name]["link_xy"] = this->link_xy;

  return true;
}

bool WaveNbAttribute::deserialize_json_v2(std::string     field_name,
                                          nlohmann::json &input_data)
{
  if (input_data[field_name].is_object() == false ||
      input_data[field_name]["value"].is_object() == false ||
      input_data[field_name]["link_xy"].is_boolean() == false ||
      input_data[field_name]["value"]["x"].is_number() == false ||
      input_data[field_name]["value"]["y"].is_number() == false)
  {
    LOG_DEBUG("Attribute %s is not a an array.", field_name.data());
    return false;
  }

  this->value.x = input_data[field_name]["value"]["x"].get<float>();
  this->value.y = input_data[field_name]["value"]["y"].get<float>();
  this->link_xy = input_data[field_name]["link_xy"].get<bool>();
  return true;
}

} // namespace hesiod
