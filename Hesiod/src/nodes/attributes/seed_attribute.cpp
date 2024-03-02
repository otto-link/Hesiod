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

SeedAttribute::SeedAttribute(int value) : value(value)
{
}

int SeedAttribute::get()
{
  return value;
}

bool SeedAttribute::render_settings(std::string label)
{
  bool has_changed = false;

  ImGui::DragInt(label.c_str(), &this->value);
  has_changed |= ImGui::IsItemDeactivatedAfterEdit();

  ImGui::SameLine();

  if (ImGui::Button("Rnd"))
  {
    this->value = (int)time(NULL);
    has_changed = true;
  }

  return has_changed;
}

bool SeedAttribute::serialize_json_v2(std::string     fieldName,
                                      nlohmann::json &outputData)
{
  outputData[fieldName] = this->value;
  return true;
}

bool SeedAttribute::deserialize_json_v2(std::string     fieldName,
                                        nlohmann::json &inputData)
{
  if (inputData[fieldName].is_number() == false)
  {
    LOG_DEBUG("Attribute %s is not a an array.", fieldName.data());
    return false;
  }

  this->value = inputData[fieldName].get<int>();
  return true;
}

} // namespace hesiod
