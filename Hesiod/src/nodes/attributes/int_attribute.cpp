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

IntAttribute::IntAttribute(int value, int vmin, int vmax)
    : value(value), vmin(vmin), vmax(vmax)
{
}

int IntAttribute::get()
{
  return value;
}

bool IntAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  ImGui::SliderInt(label.c_str(), &this->value, this->vmin, this->vmax);
  has_changed |= ImGui::IsItemDeactivatedAfterEdit();
  return has_changed;
}

bool IntAttribute::serialize_json_v2(std::string fieldName, nlohmann::json& outputData) 
{ 
  outputData[fieldName]["value"] = this->value;
  outputData[fieldName]["vmin"] = this->vmin;
  outputData[fieldName]["vmax"] = this->vmax;
  return true; 
}

bool IntAttribute::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData) 
{ 
  if(
    inputData[fieldName].is_null() == true ||
    inputData[fieldName]["value"].is_number() == false ||
    inputData[fieldName]["vmin"].is_number() == false ||
    inputData[fieldName]["vmax"].is_number() == false
  )
  {
    LOG_DEBUG("[%s] Attribute %s is not valid.", __FUNCTION__, fieldName.data());
    return false;
  }

  this->value = inputData[fieldName]["value"].get<int>();
  this->vmin = inputData[fieldName]["vmin"].get<int>();
  this->vmax = inputData[fieldName]["vmax"].get<int>();
  return true; 
}

} // namespace hesiod
