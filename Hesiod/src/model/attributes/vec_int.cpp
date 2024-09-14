/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes/attributes.hpp"

namespace hesiod
{

VecIntAttribute::VecIntAttribute() {}

VecIntAttribute::VecIntAttribute(std::vector<int> value) : value(value) {}

VecIntAttribute::VecIntAttribute(std::vector<int> value, int vmin, int vmax)
    : value(value), vmin(vmin), vmax(vmax)
{
}

std::vector<int> VecIntAttribute::get() { return value; }

nlohmann::json VecIntAttribute::json_to() const
{
  nlohmann::json json;
  json["value"] = this->value;
  json["vmin"] = this->vmin;
  json["vmax"] = this->vmax;
  return json;
}

void VecIntAttribute::json_from(nlohmann::json const &json)
{
  this->value = json["value"].get<std::vector<int>>();
  this->vmin = json["vmin"];
  this->vmax = json["vmax"];
}

} // namespace hesiod
