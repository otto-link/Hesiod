/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes/attributes.hpp"

namespace hesiod
{

VecFloatAttribute::VecFloatAttribute() {}

VecFloatAttribute::VecFloatAttribute(std::vector<float> value,
                                     float              vmin,
                                     float              vmax,
                                     std::string        fmt)
    : value(value), vmin(vmin), vmax(vmax), fmt(fmt)
{
}

std::vector<float> VecFloatAttribute::get() { return value; }

nlohmann::json VecFloatAttribute::json_to() const
{
  nlohmann::json json;
  json["value"] = this->value;
  json["vmin"] = this->vmin;
  json["vmax"] = this->vmax;
  json["fmt"] = this->fmt;
  return json;
}

void VecFloatAttribute::json_from(nlohmann::json const &json)
{
  this->value = json["value"].get<std::vector<float>>();
  this->vmin = json["vmin"];
  this->vmax = json["vmax"];
  this->fmt = json["fmt"];
}

} // namespace hesiod
