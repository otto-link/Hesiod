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

FloatAttribute::FloatAttribute(float value, float vmin, float vmax)
    : value(value), vmin(vmin), vmax(vmax)
{
}

float FloatAttribute::get()
{
  return value;
}

void FloatAttribute::set(float new_value)
{
  this->value = new_value;
}

bool FloatAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  ImGui::SliderFloat(label.c_str(),
                     &this->value,
                     this->vmin,
                     this->vmax,
                     "%.2f");
  has_changed |= ImGui::IsItemDeactivatedAfterEdit();
  return has_changed;
}

bool FloatAttribute::serialize_json_v2(std::string     field_name,
                                       nlohmann::json &output_data)
{
  output_data[field_name]["value"] = this->value;
  output_data[field_name]["vmin"] = this->vmin;
  output_data[field_name]["vmax"] = this->vmax;
  return true;
}

bool FloatAttribute::deserialize_json_v2(std::string     field_name,
                                         nlohmann::json &input_data)
{
  if (input_data[field_name].is_null() == true ||
      input_data[field_name]["value"].is_number() == false ||
      input_data[field_name]["vmin"].is_number() == false ||
      input_data[field_name]["vmax"].is_number() == false)
  {
    LOG_DEBUG("Attribute %s is not valid.", field_name.data());
    return false;
  }

  this->value = input_data[field_name]["value"].get<float>();
  this->vmin = input_data[field_name]["vmin"].get<float>();
  this->vmax = input_data[field_name]["vmax"].get<float>();
  return true;
}

} // namespace hesiod
