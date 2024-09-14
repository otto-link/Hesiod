/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

std::vector<float> ColorAttribute::get() { return value; }

nlohmann::json ColorAttribute::json_to() const
{
  nlohmann::json json;
  json["value"] = this->value;
  return json;
}

void ColorAttribute::json_from(nlohmann::json const &json)
{
  this->value = json["value"].get<std::vector<float>>();
}

} // namespace hesiod
