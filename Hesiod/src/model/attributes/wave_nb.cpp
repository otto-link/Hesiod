/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes/attributes.hpp"

namespace hesiod
{

WaveNbAttribute::WaveNbAttribute(hmap::Vec2<float> value, std::string fmt)
    : value(value), fmt(fmt)
{
}

WaveNbAttribute::WaveNbAttribute(hmap::Vec2<float> value,
                                 float             vmin,
                                 float             vmax,
                                 std::string       fmt)
    : value(value), vmin(vmin), vmax(vmax), fmt(fmt)
{
}

hmap::Vec2<float> WaveNbAttribute::get() { return value; }

nlohmann::json WaveNbAttribute::json_to() const
{
  nlohmann::json json;
  json["value.x"] = this->value.x;
  json["value.y"] = this->value.y;
  json["link_xy"] = this->link_xy;
  json["vmin"] = this->vmin;
  json["vmax"] = this->vmax;
  json["fmt"] = this->fmt;
  return json;
}

void WaveNbAttribute::json_from(nlohmann::json const &json)
{
  this->value.x = json["value.x"];
  this->value.y = json["value.y"];
  this->link_xy = json["link_xy"];
  this->vmin = json["vmin"];
  this->vmax = json["vmax"];
  this->fmt = json["fmt"];
}

} // namespace hesiod
