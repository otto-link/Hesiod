#include <string>
#include <vector>

#include "highmap/array.hpp"
#include "macro-logger/macrologger.h"

#include "hesiod/serialization.hpp"

namespace hesiod::serialization
{
#define SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(Enum, Cast)               \
  case SerializationBatchHelperElementType::Enum:                              \
  {                                                                            \
    batch_data[e->name] = *reinterpret_cast<Cast *>(e->data);                  \
    break;                                                                     \
  }

#define SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(Enum, Cast)             \
  case SerializationBatchHelperElementType::Enum:                              \
  {                                                                            \
    *reinterpret_cast<Cast *>(e->data) = batch_data[e->name].get<Cast>();      \
    break;                                                                     \
  }

SerializationBatchHelper::SerializationBatchHelper()
{
}

SerializationBatchHelper::~SerializationBatchHelper()
{
}

bool SerializationBatchHelper::serialize_json_v2(std::string     field_name,
                                                 nlohmann::json &output_data)
{
  nlohmann::json batch_data = nlohmann::json();

  for (SerializationBatchHelperElement *e : this->elements)
  {
    if (e->data == nullptr)
      continue;

    switch (e->type)
    {
      SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(BYTE, unsigned char);
      SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(CHARACTER, char);
      SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(UNSIGNED_SHORT,
                                                   unsigned short);
      SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(SHORT, short);
      SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(UNSIGNED_INT, unsigned int);
      SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(INT, int);
      SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(LONG_LONG, long long);
      SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(UNSIGNED_LONG_LONG,
                                                   unsigned long long);
      SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(FLOAT, float);
      SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(DOUBLE, double);
      SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(BOOL, bool);
    case SerializationBatchHelperElementType::STL_STRING:
    {
      batch_data[e->name] = reinterpret_cast<std::string *>(e->data)->data();
      break;
    }
    case SerializationBatchHelperElementType::OBJECT:
    {
      nlohmann::json temp_data = nlohmann::json();

      SerializationBase *base = reinterpret_cast<SerializationBase *>(e->data);

      if (base)
        base->serialize_json_v2("sub", temp_data);

      batch_data[e->name] = temp_data;
      break;
    }
    case SerializationBatchHelperElementType::OBJECT_PTR:
    {
      nlohmann::json temp_data = nlohmann::json();

      SerializationBase *base = (*reinterpret_cast<SerializationBase **>(
          e->data));

      if (base)
        base->serialize_json_v2("sub", temp_data);

      batch_data[e->name] = temp_data;
      break;
    }
    default:
      LOG_ERROR("[%s] Unimplemented type.", __FUNCTION__);
      break;
    }
  }

  output_data[field_name] = batch_data;

  return true;
}

bool SerializationBatchHelper::deserialize_json_v2(std::string     field_name,
                                                   nlohmann::json &input_data)
{
  if (input_data[field_name].is_null() ||
      input_data[field_name].is_object() == false)
  {
    LOG_ERROR("[%s] Encountered error while parsing object.", __FUNCTION__);
    return false;
  }

  nlohmann::json batch_data = input_data[field_name];

  for (SerializationBatchHelperElement *e : this->elements)
  {
    if (e->data == nullptr)
      continue;
    if (batch_data[e->name].is_null())
      continue;

    switch (e->type)
    {
      SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(BYTE, unsigned char);
      SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(CHARACTER, char);
      SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(UNSIGNED_SHORT,
                                                     unsigned short);
      SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(SHORT, short);
      SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(UNSIGNED_INT,
                                                     unsigned int);
      SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(INT, int);
      SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(LONG_LONG, long long);
      SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(UNSIGNED_LONG_LONG,
                                                     unsigned long long);
      SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(FLOAT, float);
      SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(DOUBLE, double);
      SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(BOOL, bool);
    case SerializationBatchHelperElementType::STL_STRING:
    {
      std::string deserializedString = batch_data[e->name].get<std::string>();
      reinterpret_cast<std::string *>(e->data)->swap(deserializedString);
      break;
    }
    case SerializationBatchHelperElementType::OBJECT:
    {
      nlohmann::json     temp_data = batch_data[e->name];
      SerializationBase *base = reinterpret_cast<SerializationBase *>(e->data);

      if (base)
        base->deserialize_json_v2("sub", temp_data);

      break;
    }
    case SerializationBatchHelperElementType::OBJECT_PTR:
    {
      nlohmann::json     temp_data = batch_data[e->name];
      SerializationBase *base = (*reinterpret_cast<SerializationBase **>(
          e->data));

      if (base)
        base->deserialize_json_v2("sub", temp_data);

      break;
    }
    default:
      LOG_ERROR("[%s] Unimplemented type.", __FUNCTION__);
      break;
    }
  }

  return true;
}

// SerializationBatchBase

bool SerializationBatchBase::serialize_json_v2(std::string     field_name,
                                               nlohmann::json &output_data)
{
  return this->BuildBatchHelperData().serialize_json_v2(field_name,
                                                        output_data);
}

bool SerializationBatchBase::deserialize_json_v2(std::string     field_name,
                                                 nlohmann::json &input_data)
{
  return this->BuildBatchHelperData().deserialize_json_v2(field_name,
                                                          input_data);
}

// Adapter hmap::Array

nlohmann::json adapter_hmap_array_serialize(hmap::Array a)
{
  nlohmann::json data = nlohmann::json();

  data["shape"] = adapter_hmap_serialize_vec2<int>(a.shape);
  data["vector"] = a.vector;

  return data;
}

hmap::Array adapter_hmap_array_deserialize(nlohmann::json &data)
{
  hmap::Array array = hmap::Array();

  array.set_shape(adapter_hmap_deserialize_vec2<int>(data["shape"]));
  array.vector = data["vector"].get<std::vector<float>>();

  return array;
}

} // namespace hesiod::serialization
