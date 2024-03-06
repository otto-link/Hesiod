/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "highmap/geometry.hpp"
#include "macrologger.h"
#include <vector>

#include "hesiod/attribute.hpp"

namespace hesiod
{

hmap::Cloud CloudAttribute::get()
{
  return value;
}

bool CloudAttribute::serialize_json_v2(std::string     field_name,
                                       nlohmann::json &output_data)
{
  output_data[field_name]["x"] = this->value.get_x();
  output_data[field_name]["y"] = this->value.get_y();
  output_data[field_name]["v"] = this->value.get_values();

  return true;
}

bool CloudAttribute::deserialize_json_v2(std::string     field_name,
                                         nlohmann::json &input_data)
{
  if (input_data[field_name]["x"].is_array() == false ||
      input_data[field_name]["y"].is_array() == false ||
      input_data[field_name]["v"].is_array() == false)
  {
    LOG_DEBUG("Attribute %s is not filled with arrays.", field_name.data());
    return false;
  }

  this->value = hmap::Cloud(
      input_data[field_name]["x"].get<std::vector<float>>(),
      input_data[field_name]["y"].get<std::vector<float>>(),
      input_data[field_name]["v"].get<std::vector<float>>());

  return true;
}

} // namespace hesiod
