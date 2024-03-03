#include <map>
#include <memory>

#include "macrologger.h"

#include "hesiod/attribute.hpp"

namespace hesiod
{

static std::map<AttributeType, std::string> attribute_type_to_name = {
    {AttributeType::BOOL_ATTRIBUTE, "BOOL_ATTRIBUTE"},
    {AttributeType::CLOUD_ATTRIBUTE, "CLOUD_ATTRIBUTE"},
    {AttributeType::COLOR_ATTRIBUTE, "COLOR_ATTRIBUTE"},
    {AttributeType::FILENAME_ATTRIBUTE, "FILENAME_ATTRIBUTE"},
    {AttributeType::FLOAT_ATTRIBUTE, "FLOAT_ATTRIBUTE"},
    {AttributeType::INT_ATTRIBUTE, "INT_ATTRIBUTE"},
    {AttributeType::MAP_ENUM_ATTRIBUTE, "MAP_ENUM_ATTRIBUTE"},
    {AttributeType::MATRIX_ATTRIBUTE, "MATRIX_ATTRIBUTE"},
    {AttributeType::PATH_ATTRIBUTE, "PATH_ATTRIBUTE"},
    {AttributeType::RANGE_ATTRIBUTE, "RANGE_ATTRIBUTE"},
    {AttributeType::SEED_ATTRIBUTE, "SEED_ATTRIBUTE"},
    {AttributeType::SHAPE_ATTRIBUTE, "SHAPE_ATTRIBUTE"},
    {AttributeType::VEC_FLOAT_ATTRIBUTE, "VEC_FLOAT_ATTRIBUTE"},
    {AttributeType::VEC_INT_ATTRIBUTE, "VEC_INT_ATTRIBUTE"},
    {AttributeType::WAVE_NB_ATTRIBUTE, "WAVE_NB_ATTRIBUTE"},
};

std::string AttributeInstancing::get_name_from_type(AttributeType type)
{
  if (attribute_type_to_name.contains(type) == false)
  {
    LOG_ERROR("Encountered unknown type! (%i)", static_cast<int>(type));
    return "INVALID";
  }

  return attribute_type_to_name.at(type);
}

AttributeType AttributeInstancing::get_type_from_name(std::string name)
{
  for (std::map<AttributeType, std::string>::iterator current_iterator =
           attribute_type_to_name.begin();
       current_iterator != attribute_type_to_name.end();
       current_iterator++)
  {
    if (current_iterator->second == name)
    {
      return current_iterator->first;
    }
  }

  return AttributeType::INVALID;
}

std::unique_ptr<Attribute> AttributeInstancing::create_attribute_from_type(
    AttributeType type)
{
  switch (type)
  {
  default:
  case AttributeType::INVALID:
    LOG_ERROR("Encountered unknown type! (%i)", static_cast<int>(type));
    return nullptr;
  case AttributeType::BOOL_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new BoolAttribute());
  case AttributeType::CLOUD_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new CloudAttribute());
  case AttributeType::COLOR_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new ColorAttribute());
  case AttributeType::FILENAME_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new FilenameAttribute());
  case AttributeType::FLOAT_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new FloatAttribute());
  case AttributeType::INT_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new IntAttribute());
  case AttributeType::MAP_ENUM_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new MapEnumAttribute());
  case AttributeType::MATRIX_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new MatrixAttribute());
  case AttributeType::PATH_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new PathAttribute());
  case AttributeType::RANGE_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new RangeAttribute());
  case AttributeType::SEED_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new SeedAttribute());
  case AttributeType::SHAPE_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new ShapeAttribute());
  case AttributeType::VEC_FLOAT_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new VecFloatAttribute());
  case AttributeType::VEC_INT_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new VecIntAttribute());
  case AttributeType::WAVE_NB_ATTRIBUTE:
    return std::unique_ptr<Attribute>(new WaveNbAttribute());
  }
}

} // namespace hesiod
