/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <map>
#include <memory>

#include "hesiod/model/attributes/attributes.hpp"

namespace hesiod
{

static std::map<AttributeType, std::string> attribute_type_to_name = {
    {AttributeType::BOOL, "BOOL"},
    {AttributeType::CLOUD, "CLOUD"},
    {AttributeType::COLOR, "COLOR"},
    {AttributeType::FILENAME, "FILENAME"},
    {AttributeType::FLOAT, "FLOAT"},
    {AttributeType::INT, "INT"},
    {AttributeType::MAP_ENUM, "MAP_ENUM"},
    {AttributeType::MATRIX, "MATRIX"},
    {AttributeType::PATH, "PATH"},
    {AttributeType::RANGE, "RANGE"},
    {AttributeType::SEED, "SEED"},
    {AttributeType::SHAPE, "SHAPE"},
    {AttributeType::VEC_FLOAT, "VEC_FLOAT"},
    {AttributeType::VEC_INT, "VEC_INT"},
    {AttributeType::WAVE_NB, "WAVE_NB"},
};

std::string get_attribute_name_from_type(AttributeType type)
{
  if (attribute_type_to_name.contains(type) == false)
  {
    HLOG->critical("encountered unknown type for attribute ({})", static_cast<int>(type));
    throw std::runtime_error("attribute encountered unknown type");
  }

  return attribute_type_to_name.at(type);
}

AttributeType get_attribute_type_from_name(std::string name)
{
  for (auto [type, search_name] : attribute_type_to_name)
    if (name == search_name)
      return type;

  return AttributeType::INVALID;
}

} // namespace hesiod
