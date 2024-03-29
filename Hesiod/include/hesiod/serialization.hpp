#pragma once
#include <cstring>
#include <string>

#include "highmap/array.hpp"
#include "highmap/vector.hpp"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

#define SERIALIZATION_V2_IMPLEMENT_BASE()                                      \
  bool serialize_json_v2(std::string field_name, nlohmann::json &output_data); \
  bool deserialize_json_v2(std::string field_name, nlohmann::json &input_data);

#define SERIALIZATION_V2_IMPLEMENT_BATCH_BASE()                                \
  hesiod::serialization::SerializationBatchHelper BuildBatchHelperData();

namespace hesiod::serialization
{
enum class SerializationType
{
  PLAIN,
  BJDATA,
  BSON,
  CBOR,
  MESSAGEPACK,
  UBJSON
};

enum class SerializationBatchHelperElementType : char
{
  BYTE = 0,
  CHARACTER = 1,
  UNSIGNED_SHORT = 2,
  SHORT = 3,
  UNSIGNED_INT = 4,
  INT = 5,
  LONG_LONG = 6,
  UNSIGNED_LONG_LONG = 7,
  FLOAT = 8,
  DOUBLE = 9,
  STL_STRING = 10,
  OBJECT = 11,
  OBJECT_PTR = 12,
  BOOL = 13,
  INVALID = 13
};

class SerializationBase
{
public:
  virtual ~SerializationBase() = default;
  virtual bool serialize_json_v2(std::string, nlohmann::json &)
  {
    return false;
  }
  virtual bool deserialize_json_v2(std::string, nlohmann::json &)
  {
    return false;
  }
};

class SerializationBatchHelperElement
{
public:
  SerializationBatchHelperElement()
      : name(), data(), type(SerializationBatchHelperElementType::INVALID)
  {
  }

  SerializationBatchHelperElement(SerializationBatchHelperElement &S)
  {
    this->operator=(S);
  }

  ~SerializationBatchHelperElement() = default;

  SerializationBatchHelperElement &operator=(SerializationBatchHelperElement s)
  {
    this->name = s.name;
    this->data = s.data;
    this->type = s.type;

    return *this;
  }

public:
  std::string                         name;
  void                               *data;
  SerializationBatchHelperElementType type;
};

class SerializationBatchHelper : SerializationBase
{
public:
  SerializationBatchHelper();
  ~SerializationBatchHelper();

  bool serialize_json_v2(std::string field_name, nlohmann::json &output_data);
  bool deserialize_json_v2(std::string field_name, nlohmann::json &input_data);

  // Wrappers for the ease of use

  SerializationBatchHelper &AddByte(std::string key, unsigned char *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::BYTE;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddCharacter(std::string key, char *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::CHARACTER;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddUnsignedShort(std::string     key,
                                             unsigned short *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::UNSIGNED_SHORT;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddShort(std::string key, short *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::SHORT;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddUnsignedInt(std::string key, unsigned int *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::UNSIGNED_INT;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddInt(std::string key, int *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::INT;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddLongLong(std::string key, long long *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::LONG_LONG;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddUnsignedLongLong(std::string         key,
                                                unsigned long long *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::UNSIGNED_LONG_LONG;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddFloat(std::string key, float *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::FLOAT;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddDouble(std::string key, double *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::DOUBLE;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddStlString(std::string key, std::string *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::STL_STRING;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddObject(std::string key, SerializationBase *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::OBJECT;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddObjectPtr(std::string        key,
                                         SerializationBase *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::OBJECT_PTR;
    this->elements.push_back(e);

    return *this;
  }

  SerializationBatchHelper &AddBool(std::string key, bool *value)
  {
    SerializationBatchHelperElement *e = new SerializationBatchHelperElement();
    e->name = key;
    e->data = value;
    e->type = SerializationBatchHelperElementType::BOOL;
    this->elements.push_back(e);

    return *this;
  }

private:
  std::vector<SerializationBatchHelperElement *> elements;
};

class SerializationBatchBase : public SerializationBase
{
public:
  SerializationBatchBase() = default;
  ~SerializationBatchBase() = default;

  virtual SerializationBatchHelper BuildBatchHelperData()
  {
    return SerializationBatchHelper();
  }

  bool serialize_json_v2(std::string field_name, nlohmann::json &output_data);
  bool deserialize_json_v2(std::string field_name, nlohmann::json &input_data);
};

// Adapter hmap::Vec2

template <typename T>
inline nlohmann::json adapter_hmap_serialize_vec2(hmap::Vec2<T> v)
{
  nlohmann::json data = nlohmann::json();

  data["x"] = v.x;
  data["y"] = v.y;

  return data;
}

template <typename T>
inline hmap::Vec2<T> adapter_hmap_deserialize_vec2(nlohmann::json &data)
{
  if (data.is_object() == false)
    return hmap::Vec2<T>();

  return hmap::Vec2<T>(data["x"].get<T>(), data["y"].get<T>());
}

// Adapter hmap::Vec4

template <typename T>
inline nlohmann::json adapter_hmap_serialize_vec4(hmap::Vec4<T> v)
{
  nlohmann::json data = nlohmann::json();

  data["a"] = v.a;
  data["b"] = v.b;
  data["c"] = v.c;
  data["d"] = v.d;

  return data;
}

template <typename T>
inline hmap::Vec4<T> adapter_hmap_deserialize_vec4(nlohmann::json &data)
{
  if (data.is_object() == false)
    return hmap::Vec4<T>();

  return hmap::Vec4<T>(data["a"].get<T>(),
                       data["b"].get<T>(),
                       data["c"].get<T>(),
                       data["d"].get<T>());
}

// Adapter hmap::Array

nlohmann::json adapter_hmap_array_serialize(hmap::Array a);
hmap::Array    adapter_hmap_array_deserialize(nlohmann::json &data);

} // namespace hesiod::serialization