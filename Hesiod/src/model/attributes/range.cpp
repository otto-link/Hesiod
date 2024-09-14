/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

RangeAttribute::RangeAttribute(hmap::Vec2<float> value, std::string fmt)
    : value(value), fmt(fmt)
{
  this->vmin = 2.f * this->value.x - this->value.y;
  this->vmax = 2.f * this->value.y - this->value.x;
}

RangeAttribute::RangeAttribute(hmap::Vec2<float> value,
                               float             vmin,
                               float             vmax,
                               std::string       fmt)
    : value(value), vmin(vmin), vmax(vmax), fmt(fmt)
{
}

RangeAttribute::RangeAttribute(std::string fmt) : fmt(fmt)
{
  this->vmin = 2.f * this->value.x - this->value.y;
  this->vmax = 2.f * this->value.y - this->value.x;
}

hmap::Vec2<float> RangeAttribute::get() { return value; }

nlohmann::json RangeAttribute::json_to() const
{
  nlohmann::json json;
  json["value.x"] = this->value.x;
  json["value.y"] = this->value.y;
  json["vmin"] = this->vmin;
  json["vmax"] = this->vmax;
  json["fmt"] = fmt;
  return json;
}

void RangeAttribute::json_from(nlohmann::json const &json)
{
  this->value.x = json["value.x"];
  this->value.y = json["value.y"];
  this->vmin = json["vmin"];
  this->vmax = json["vmax"];
  this->fmt = json["fmt"];
}

} // namespace hesiod
