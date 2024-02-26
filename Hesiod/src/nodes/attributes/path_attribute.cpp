/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "highmap/geometry.hpp"
#include "macrologger.h"
#include <imgui.h>
#include <vector>

#include "hesiod/attribute.hpp"
#include "hesiod/gui.hpp"

namespace hesiod
{

hmap::Path PathAttribute::get()
{
  return value;
}

bool PathAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  ImGui::TextUnformatted(label.c_str());

  ImGui::Checkbox("closed", &this->value.closed);
  has_changed |= ImGui::IsItemDeactivatedAfterEdit();

  ImGui::SameLine();

  if (ImGui::Button("reverse"))
  {
    this->value.reverse();
    has_changed = true;
  }

  has_changed |= hesiod::gui::canvas_path_editor(this->value);
  return has_changed;
}

bool PathAttribute::serialize_json_v2(std::string fieldName, nlohmann::json& outputData) 
{ 
  outputData[fieldName]["x"] = this->value.get_x();
  outputData[fieldName]["y"] = this->value.get_y();
  outputData[fieldName]["v"] = this->value.get_values();
  outputData[fieldName]["closed"] = this->value.closed;

  return true; 
}

bool PathAttribute::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData) 
{ 
  if(
    inputData[fieldName].is_object() == false ||
    inputData[fieldName]["x"].is_array() == false ||
    inputData[fieldName]["y"].is_array() == false ||
    inputData[fieldName]["v"].is_array() == false ||
    inputData[fieldName]["closed"].is_boolean() == false
  )
  {
    LOG_DEBUG("Attribute %s is invalid.", fieldName.data());
    return false;
  }

  this->value = hmap::Path(
    inputData[fieldName]["x"].get<std::vector<float>>(),
    inputData[fieldName]["y"].get<std::vector<float>>(),
    inputData[fieldName]["v"].get<std::vector<float>>()
  );
  this->value.closed = inputData[fieldName]["closed"].get<bool>();

  return true; 
}

} // namespace hesiod
