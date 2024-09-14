/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

ColorGradientAttribute::ColorGradientAttribute() {}

ColorGradientAttribute::ColorGradientAttribute(std::vector<std::vector<float>> value)
    : value(value)
{
}

std::vector<std::vector<float>> ColorGradientAttribute::get() { return value; }

nlohmann::json ColorGradientAttribute::json_to() const
{
  nlohmann::json json;
  json["value"] = this->value;
  return json;
}

void ColorGradientAttribute::json_from(nlohmann::json const &json)
{
  this->value = json["value"].get<std::vector<std::vector<float>>>();
}

} // namespace hesiod
