/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

SeedAttribute::SeedAttribute(uint value) : value(value) {}

uint SeedAttribute::get() { return value; }

nlohmann::json SeedAttribute::json_to() const
{
  nlohmann::json json;
  json["value"] = this->value;
  return json;
}

void SeedAttribute::json_from(nlohmann::json const &json) { this->value = json["value"]; }

} // namespace hesiod
