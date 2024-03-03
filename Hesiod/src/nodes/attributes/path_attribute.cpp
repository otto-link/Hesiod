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

bool PathAttribute::serialize_json_v2(std::string     field_name,
                                      nlohmann::json &output_data)
{
  output_data[field_name]["x"] = this->value.get_x();
  output_data[field_name]["y"] = this->value.get_y();
  output_data[field_name]["v"] = this->value.get_values();
  output_data[field_name]["closed"] = this->value.closed;

  return true;
}

bool PathAttribute::deserialize_json_v2(std::string     field_name,
                                        nlohmann::json &input_data)
{
  if (input_data[field_name].is_object() == false ||
      input_data[field_name]["x"].is_array() == false ||
      input_data[field_name]["y"].is_array() == false ||
      input_data[field_name]["v"].is_array() == false ||
      input_data[field_name]["closed"].is_boolean() == false)
  {
    LOG_DEBUG("Attribute %s is invalid.", field_name.data());
    return false;
  }

  this->value = hmap::Path(
      input_data[field_name]["x"].get<std::vector<float>>(),
      input_data[field_name]["y"].get<std::vector<float>>(),
      input_data[field_name]["v"].get<std::vector<float>>());
  this->value.closed = input_data[field_name]["closed"].get<bool>();

  return true;
}

} // namespace hesiod
