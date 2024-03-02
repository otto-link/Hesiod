/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "highmap/vector.hpp"
#include "macrologger.h"
#include <imgui.h>

#include "hesiod/attribute.hpp"
#include "hesiod/gui.hpp"

namespace hesiod
{

RangeAttribute::RangeAttribute(hmap::Vec2<float> value) : value(value)
{
}

RangeAttribute::RangeAttribute(bool activate) : activate(activate)
{
}

hmap::Vec2<float> RangeAttribute::get()
{
  return value;
}

bool RangeAttribute::render_settings(std::string label)
{
  bool has_changed = false;

  if (ImGui::Checkbox(label.c_str(), &this->activate))
    has_changed = true;

  // if (this->activate)
  {
    ImGui::Indent();
    if (hesiod::gui::slider_vmin_vmax(this->value.x, this->value.y))
      has_changed = true;
    ImGui::Unindent();
  }

  return has_changed;
}

bool RangeAttribute::serialize_json_v2(std::string     fieldName,
                                       nlohmann::json &outputData)
{
  outputData[fieldName]["x"] = this->value.x;
  outputData[fieldName]["y"] = this->value.y;
  outputData[fieldName]["activate"] = this->activate;
  return true;
}

bool RangeAttribute::deserialize_json_v2(std::string     fieldName,
                                         nlohmann::json &inputData)
{
  if (inputData[fieldName].is_object() == false ||
      inputData[fieldName]["x"].is_number() == false ||
      inputData[fieldName]["y"].is_number() == false ||
      inputData[fieldName]["activate"].is_boolean() == false)
  {
    LOG_DEBUG("Attribute %s is invalid.", fieldName.data());
    return false;
  }

  this->value = hmap::Vec2<float>(inputData[fieldName]["x"].get<float>(),
                                  inputData[fieldName]["y"].get<float>());

  this->activate = inputData[fieldName]["activate"].get<bool>();
  return true;
}

} // namespace hesiod
