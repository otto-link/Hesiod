/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/logger.hpp"
#include "hesiod/model/attributes/attributes.hpp"

namespace hesiod
{

MapEnumAttribute::MapEnumAttribute(std::map<std::string, int> value) : value(value)
{
  // default choice is first item in the list
  this->choice = this->value.begin()->first;
}

MapEnumAttribute::MapEnumAttribute(std::map<std::string, int> value, std::string choice)
    : value(value), choice(choice)
{
  if (!this->value.contains(this->choice))
  {
    HLOG->critical("initial choice not in the available keys ({})", this->choice);
    throw std::runtime_error("initial choice not in the available keys");
  }
}

int MapEnumAttribute::get() { return this->value.at(this->choice); }

std::map<std::string, int> MapEnumAttribute::get_map() { return this->value; }

void MapEnumAttribute::set(std::string new_choice) { this->choice = new_choice; }

nlohmann::json MapEnumAttribute::json_to() const
{
  nlohmann::json json = Attribute::json_to();
  json["choice"] = this->choice;
  return json;
}

void MapEnumAttribute::json_from(nlohmann::json const &json)
{
  Attribute::json_from(json);
  this->choice = json["choice"];
}

} // namespace hesiod
