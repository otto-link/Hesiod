#include "hesiod/serialization.hpp"
#include "macro-logger/macrologger.h"
#include <string>

namespace hesiod::serialization
{
#define SERIALIZATION_BATCH_IMPLEMENT_SERIALIZE_CASE(Enum, Cast)               \
  case SerializationBatchHelperElementType::Enum:                              \
  {                                                                            \
    batchData[e->name] = *reinterpret_cast<Cast *>(e->data);                   \
    break;                                                                     \
  }

#define SERIALIZATION_BATCH_IMPLEMENT_DESERIALIZE_CASE(Enum, Cast)             \
  case SerializationBatchHelperElementType::Enum:                              \
  {                                                                            \
    *reinterpret_cast<Cast *>(e->data) = batchData[e->name].get<Cast>();       \
    break;                                                                     \
  }

SerializationBatchHelper::SerializationBatchHelper()
{
}

SerializationBatchHelper::~SerializationBatchHelper()
{
}

bool SerializationBatchHelper::serialize_json_v2(std::string     fieldName,
                                                 nlohmann::json &outputData)
{
  nlohmann::json batchData = nlohmann::json();

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
    case SerializationBatchHelperElementType::STL_STRING:
    {
      batchData[e->name] = reinterpret_cast<std::string *>(e->data)->data();
      break;
    }
    case SerializationBatchHelperElementType::OBJECT:
    {
      nlohmann::json tempData = nlohmann::json();

      SerializationBase *base = reinterpret_cast<SerializationBase *>(e->data);

      if (base)
        base->serialize_json_v2("sub", tempData);

      batchData[e->name] = tempData;
      break;
    }
    case SerializationBatchHelperElementType::OBJECT_PTR:
    {
      nlohmann::json tempData = nlohmann::json();

      SerializationBase *base = (*reinterpret_cast<SerializationBase **>(
          e->data));

      if (base)
        base->serialize_json_v2("sub", tempData);

      batchData[e->name] = tempData;
      break;
    }
    default:
      LOG_ERROR("[%s] Unimplemented type.", __FUNCTION__);
      break;
    }
  }

  outputData[fieldName] = batchData;

  return true;
}

bool SerializationBatchHelper::deserialize_json_v2(std::string     fieldName,
                                                   nlohmann::json &inputData)
{
  if (inputData[fieldName].is_null() ||
      inputData[fieldName].is_object() == false)
  {
    LOG_ERROR("[%s] Encountered error while parsing object.", __FUNCTION__);
    return false;
  }

  nlohmann::json batchData = inputData[fieldName];

  for (SerializationBatchHelperElement *e : this->elements)
  {
    if (e->data == nullptr)
      continue;
    if (batchData[e->name].is_null())
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
    case SerializationBatchHelperElementType::STL_STRING:
    {
      std::string deserializedString = batchData[e->name].get<std::string>();
      reinterpret_cast<std::string *>(e->data)->swap(deserializedString);
      break;
    }
    case SerializationBatchHelperElementType::OBJECT:
    {
      nlohmann::json     tempData = batchData[e->name];
      SerializationBase *base = reinterpret_cast<SerializationBase *>(e->data);

      if (base)
        base->deserialize_json_v2("sub", tempData);

      break;
    }
    case SerializationBatchHelperElementType::OBJECT_PTR:
    {
      nlohmann::json     tempData = batchData[e->name];
      SerializationBase *base = (*reinterpret_cast<SerializationBase **>(
          e->data));

      if (base)
        base->deserialize_json_v2("sub", tempData);

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

bool SerializationBatchBase::serialize_json_v2(std::string     fieldName,
                                               nlohmann::json &outputData)
{
  return this->BuildBatchHelperData().serialize_json_v2(fieldName, outputData);
}

bool SerializationBatchBase::deserialize_json_v2(std::string     fieldName,
                                                 nlohmann::json &inputData)
{
  return this->BuildBatchHelperData().deserialize_json_v2(fieldName, inputData);
}

} // namespace hesiod::serialization