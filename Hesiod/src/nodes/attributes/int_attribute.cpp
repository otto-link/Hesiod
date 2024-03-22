/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "macrologger.h"

#include "hesiod/attribute.hpp"

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

bool IntAttribute::serialize_json_v2(std::string     field_name,
                                     nlohmann::json &output_data)
{
  output_data[field_name]["value"] = this->value;
  output_data[field_name]["vmin"] = this->vmin;
  output_data[field_name]["vmax"] = this->vmax;
  return true;
}

bool IntAttribute::deserialize_json_v2(std::string     field_name,
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

  this->value = input_data[field_name]["value"].get<int>();
  this->vmin = input_data[field_name]["vmin"].get<int>();
  this->vmax = input_data[field_name]["vmax"].get<int>();
  return true;
}

} // namespace hesiod
