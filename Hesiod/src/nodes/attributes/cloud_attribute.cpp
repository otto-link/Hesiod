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

hmap::Cloud CloudAttribute::get()
{
  return value;
}

bool CloudAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  ImGui::TextUnformatted(label.c_str());
  has_changed |= hesiod::gui::canvas_cloud_editor(this->value);
  return has_changed;
}

bool CloudAttribute::serialize_json_v2(std::string fieldName, nlohmann::json& outputData) 
{ 
  outputData[fieldName]["x"] = this->value.get_x();
  outputData[fieldName]["y"] = this->value.get_y();
  outputData[fieldName]["v"] = this->value.get_values();
  
  return true; 
}

bool CloudAttribute::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData) 
{ 
  if(inputData[fieldName]["x"].is_array() == false || 
     inputData[fieldName]["y"].is_array() == false ||
     inputData[fieldName]["v"].is_array() == false
  )
  {
    LOG_DEBUG("Attribute %s is not filled with arrays.", fieldName.data());
    return false;
  }

  this->value = hmap::Cloud(
    inputData[fieldName]["x"].get<std::vector<float>>(),
    inputData[fieldName]["y"].get<std::vector<float>>(),
    inputData[fieldName]["v"].get<std::vector<float>>()
  );

  return true; 
}

} // namespace hesiod
