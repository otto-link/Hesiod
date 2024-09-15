/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes/attributes.hpp"

namespace hesiod
{

FloatAttribute::FloatAttribute(float value, float vmin, float vmax, std::string fmt)
    : value(value), vmin(vmin), vmax(vmax), fmt(fmt)
{
}

float FloatAttribute::get() { return value; }

nlohmann::json FloatAttribute::json_to() const
{
  nlohmann::json json = Attribute::json_to();
  json["value"] = this->value;
  json["vmin"] = this->vmin;
  json["vmax"] = this->vmax;
  json["fmt"] = this->fmt;
  return json;
}

void FloatAttribute::json_from(nlohmann::json const &json)
{
  Attribute::json_from(json);
  this->value = json["value"];
  this->vmin = json["vmin"];
  this->vmax = json["vmax"];
  this->fmt = json["fmt"];
}

} // namespace hesiod
