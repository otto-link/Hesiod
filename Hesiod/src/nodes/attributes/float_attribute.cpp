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

bool FloatAttribute::serialize_json_v2(std::string     fieldName,
                                       nlohmann::json &outputData)
{
  outputData[fieldName]["value"] = this->value;
  outputData[fieldName]["vmin"] = this->vmin;
  outputData[fieldName]["vmax"] = this->vmax;
  return true;
}

bool FloatAttribute::deserialize_json_v2(std::string     fieldName,
                                         nlohmann::json &inputData)
{
  if (inputData[fieldName].is_null() == true ||
      inputData[fieldName]["value"].is_number() == false ||
      inputData[fieldName]["vmin"].is_number() == false ||
      inputData[fieldName]["vmax"].is_number() == false)
  {
    LOG_DEBUG("Attribute %s is not valid.", fieldName.data());
    return false;
  }

  this->value = inputData[fieldName]["value"].get<float>();
  this->vmin = inputData[fieldName]["vmin"].get<float>();
  this->vmax = inputData[fieldName]["vmax"].get<float>();
  return true;
}

} // namespace hesiod
