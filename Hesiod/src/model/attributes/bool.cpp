/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes/attributes.hpp"

namespace hesiod
{

BoolAttribute::BoolAttribute(bool value, std::string label)
    : Attribute(label), value(value)
{
}

bool BoolAttribute::get() { return value; }

nlohmann::json BoolAttribute::json_to() const
{
  nlohmann::json json = Attribute::json_to();
  json["value"] = this->value;
  return json;
}

void BoolAttribute::json_from(nlohmann::json const &json)
{
  Attribute::json_from(json);
  this->value = json["value"];
}

} // namespace hesiod
