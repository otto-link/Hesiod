/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

StringAttribute::StringAttribute(std::string value) : value(value) {}

std::string StringAttribute::get() { return value; }

nlohmann::json StringAttribute::json_to() const
{
  nlohmann::json json;
  json["value"] = this->value;
  return json;
}

void StringAttribute::json_from(nlohmann::json const &json)
{
  this->value = json["value"];
}

} // namespace hesiod
