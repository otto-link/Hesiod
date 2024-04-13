/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

MapEnumAttribute::MapEnumAttribute(std::map<std::string, int> value) : value(value)
{
  // default choice is first item in the list
  this->choice = this->value.begin()->first;
}

int MapEnumAttribute::get() { return this->value.at(this->choice); }

std::map<std::string, int> MapEnumAttribute::get_map() { return this->value; }

void MapEnumAttribute::set(std::string new_choice) { this->choice = new_choice; }

} // namespace hesiod
