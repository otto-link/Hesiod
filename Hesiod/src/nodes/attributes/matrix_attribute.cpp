/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "macrologger.h"
#include <vector>

#include "hesiod/attribute.hpp"

namespace hesiod
{

MatrixAttribute::MatrixAttribute()
{
}

std::vector<std::vector<float>> MatrixAttribute::get()
{
  return value;
}

bool MatrixAttribute::serialize_json_v2(std::string     field_name,
                                        nlohmann::json &output_data)
{
  output_data[field_name] = this->value;
  return true;
}

bool MatrixAttribute::deserialize_json_v2(std::string     field_name,
                                          nlohmann::json &input_data)
{
  if (input_data[field_name].is_array() == false)
  {
    LOG_DEBUG("Attribute %s is not a an array.", field_name.data());
    return false;
  }

  this->value = input_data[field_name].get<std::vector<std::vector<float>>>();
  return true;
}

} // namespace hesiod
