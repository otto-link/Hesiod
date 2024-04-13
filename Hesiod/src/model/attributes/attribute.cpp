#include <map>
#include <memory>

#include "macrologger.h"

#include "hesiod/model/attributes.hpp"

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
    LOG_ERROR("Encountered unknown type! (%i)", static_cast<int>(type));
    return "INVALID";
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
