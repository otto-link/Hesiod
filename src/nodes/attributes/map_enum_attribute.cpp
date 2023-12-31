/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "macrologger.h"
#include <imgui.h>

#include "hesiod/attribute.hpp"
#include "hesiod/gui.hpp"

namespace hesiod
{

MapEnumAttribute::MapEnumAttribute(std::map<std::string, int> value)
    : value(value)
{
  // default choice is first item in the list
  this->choice = this->value.begin()->first;
}

int MapEnumAttribute::get()
{
  return this->value.at(this->choice);
}

std::map<std::string, int> MapEnumAttribute::get_map()
{
  return this->value;
}

void MapEnumAttribute::set(std::string new_choice)
{
  this->choice = new_choice;
}

bool MapEnumAttribute::render_settings(std::string label)
{
  bool has_changed = false;

  int choice_index = this->value.at(this->choice);

  has_changed |= hesiod::gui::listbox_map_enum(this->value,
                                               choice_index,
                                               128.f);

  // reverse map to get the key (string) based on the value chosen (int)
  for (auto &[k, v] : this->value)
    if (v == choice_index)
    {
      this->choice = k;
      break;
    }

  return has_changed;
}

} // namespace hesiod
