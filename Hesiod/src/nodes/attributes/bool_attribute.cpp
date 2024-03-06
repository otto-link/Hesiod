/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "macrologger.h"

#include "hesiod/attribute.hpp"

namespace hesiod
{

BoolAttribute::BoolAttribute(bool value) : value(value)
{
}

bool BoolAttribute::get()
{
  return value;
}

bool BoolAttribute::serialize_json_v2(std::string     field_name,
                                      nlohmann::json &output_data)
{
  output_data[field_name] = this->value;
  return true;
}

bool BoolAttribute::deserialize_json_v2(std::string     field_name,
                                        nlohmann::json &input_data)
{
  if (input_data[field_name].is_boolean() == false)
  {
    LOG_DEBUG("Attribute %s is not a boolean.", field_name.data());
    return false;
  }

  this->value = input_data[field_name].get<bool>();
  return true;
}

} // namespace hesiod
