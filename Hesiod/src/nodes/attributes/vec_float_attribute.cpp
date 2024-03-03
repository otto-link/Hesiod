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

VecFloatAttribute::VecFloatAttribute()
{
}

VecFloatAttribute::VecFloatAttribute(std::vector<float> value) : value(value)
{
}

std::vector<float> VecFloatAttribute::get()
{
  return value;
}

bool VecFloatAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  ImGui::TextUnformatted(label.c_str());
  ImGui::Indent();
  has_changed |=
      hesiod::gui::drag_float_vector(this->value, true, true, 0.f, 1.f);
  ImGui::Unindent();
  return has_changed;
}

bool VecFloatAttribute::serialize_json_v2(std::string     field_name,
                                          nlohmann::json &output_data)
{
  output_data[field_name] = this->value;
  return true;
}

bool VecFloatAttribute::deserialize_json_v2(std::string     field_name,
                                            nlohmann::json &input_data)
{
  if (input_data[field_name].is_array() == false)
  {
    LOG_DEBUG("Attribute %s is not a an array.", field_name.data());
    return false;
  }

  this->value = input_data[field_name].get<std::vector<float>>();
  return true;
}

} // namespace hesiod
