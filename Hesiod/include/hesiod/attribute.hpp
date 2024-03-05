/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file attribute.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <memory>
#include <stdexcept>

#include "highmap.hpp"
#include "macrologger.h"
#include "nlohmann/json.hpp"
#include "serialization.hpp"

// clang-format off
#define NEW_ATTR_BOOL(a) std::make_unique<hesiod::BoolAttribute>(a)
#define NEW_ATTR_CLOUD() std::make_unique<hesiod::CloudAttribute>()
#define NEW_ATTR_COLOR() std::make_unique<hesiod::ColorAttribute>()
#define NEW_ATTR_FILENAME(a) std::make_unique<hesiod::FilenameAttribute>(a)
#define NEW_ATTR_FLOAT(a, b, c) std::make_unique<hesiod::FloatAttribute>(a, b, c)
#define NEW_ATTR_INT(a, b, c) std::make_unique<hesiod::IntAttribute>(a, b, c)
#define NEW_ATTR_MAPENUM(a) std::make_unique<hesiod::MapEnumAttribute>(a)
#define NEW_ATTR_MATRIX() std::make_unique<hesiod::MatrixAttribute>()
#define NEW_ATTR_PATH() std::make_unique<hesiod::PathAttribute>()
#define NEW_ATTR_RANGE(a) std::make_unique<hesiod::RangeAttribute>(a)
#define NEW_ATTR_SEED(a) std::make_unique<hesiod::SeedAttribute>(a)
#define NEW_ATTR_SHAPE() std::make_unique<hesiod::ShapeAttribute>()
#define NEW_ATTR_VECFLOAT(a) std::make_unique<hesiod::VecFloatAttribute>(a)
#define NEW_ATTR_VECINT(a) std::make_unique<hesiod::VecIntAttribute>(a)
#define NEW_ATTR_WAVENB(a) std::make_unique<hesiod::WaveNbAttribute>(a)

#define GET_ATTR_BOOL(s) this->attr.at(s)->get_ref<BoolAttribute>()->get()
#define GET_ATTR_CLOUD(s) this->attr.at(s)->get_ref<CloudAttribute>()->get()
#define GET_ATTR_COLOR(s) this->attr.at(s)->get_ref<ColorAttribute>()->get()
#define GET_ATTR_FILENAME(s) this->attr.at(s)->get_ref<FilenameAttribute>()->get()
#define GET_ATTR_FLOAT(s) this->attr.at(s)->get_ref<FloatAttribute>()->get()
#define GET_ATTR_INT(s) this->attr.at(s)->get_ref<IntAttribute>()->get()
#define GET_ATTR_MAPENUM(s) this->attr.at(s)->get_ref<MapEnumAttribute>()->get()
#define GET_ATTR_MATRIX(s) this->attr.at(s)->get_ref<MatrixAttribute>()->get()
#define GET_ATTR_PATH(s) this->attr.at(s)->get_ref<PathAttribute>()->get()
#define GET_ATTR_RANGE(s) this->attr.at(s)->get_ref<RangeAttribute>()->get()
#define GET_ATTR_SEED(s) this->attr.at(s)->get_ref<SeedAttribute>()->get()
#define GET_ATTR_SHAPE(s) this->attr.at(s)->get_ref<ShapeAttribute>()->get()
#define GET_ATTR_VECFLOAT(s) this->attr.at(s)->get_ref<VecFloatAttribute>()->get()
#define GET_ATTR_VECINT(s) this->attr.at(s)->get_ref<VecIntAttribute>()->get()
#define GET_ATTR_WAVENB(s) this->attr.at(s)->get_ref<WaveNbAttribute>()->get()

#define GET_ATTR_REF_FLOAT(s) this->attr.at(s)->get_ref<FloatAttribute>()
#define GET_ATTR_REF_RANGE(s) this->attr.at(s)->get_ref<RangeAttribute>()
#define GET_ATTR_REF_SHAPE(s) this->attr.at(s)->get_ref<ShapeAttribute>()
// clang-format on

namespace hesiod
{

enum class AttributeType
{
  INVALID = 0,
  BOOL_ATTRIBUTE = 1,
  CLOUD_ATTRIBUTE = 2,
  COLOR_ATTRIBUTE = 3,
  FILENAME_ATTRIBUTE = 4,
  FLOAT_ATTRIBUTE = 5,
  INT_ATTRIBUTE = 6,
  MAP_ENUM_ATTRIBUTE = 7,
  MATRIX_ATTRIBUTE = 8,
  PATH_ATTRIBUTE = 9,
  RANGE_ATTRIBUTE = 10,
  SEED_ATTRIBUTE = 11,
  SHAPE_ATTRIBUTE = 12,
  VEC_FLOAT_ATTRIBUTE = 13,
  VEC_INT_ATTRIBUTE = 14,
  WAVE_NB_ATTRIBUTE = 15
};

class Attribute : public serialization::SerializationBase
{
public:
  Attribute() = default;
  virtual AttributeType get_type()
  {
    return AttributeType::INVALID;
  }
  virtual bool render_settings(std::string) = 0;

  template <class T = void> T *get_ref()
  {
    T *ptr = dynamic_cast<T *>(this);
    if (ptr)
      return ptr;
    else
    {
      LOG_ERROR("in Attribute, trying to get an attribute type which is not "
                "compatible with the current instance. Get type is: [%s]",
                typeid(T).name());
      throw std::runtime_error("wrong type");
    }
  }
};

// --- Derived

class BoolAttribute : public Attribute
{
public:
  BoolAttribute() = default;
  BoolAttribute(bool value);
  bool          get();
  AttributeType get_type()
  {
    return AttributeType::BOOL_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  bool value = true;
};

class CloudAttribute : public Attribute
{
public:
  CloudAttribute() = default;
  hmap::Cloud   get();
  AttributeType get_type()
  {
    return AttributeType::CLOUD_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  hmap::Cloud value;
};

class ColorAttribute : public Attribute
{
public:
  ColorAttribute() = default;
  std::vector<float> get();
  AttributeType      get_type()
  {
    return AttributeType::COLOR_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  std::vector<float> value = {1.f, 1.f, 1.f};
};

class FilenameAttribute : public Attribute
{
public:
  FilenameAttribute() = default;
  FilenameAttribute(std::string value);
  std::string   get();
  AttributeType get_type()
  {
    return AttributeType::FILENAME_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  std::string value = "";
};

class FloatAttribute : public Attribute
{
public:
  FloatAttribute() = default;
  FloatAttribute(float value, float vmin, float vmax);
  float         get();
  void          set(float new_value);
  AttributeType get_type()
  {
    return AttributeType::FLOAT_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  float value = 1.f;
  float vmin = 0.f;
  float vmax = 1.f;
  bool  activate = true;
};

class IntAttribute : public Attribute
{
public:
  IntAttribute() = default;
  IntAttribute(int value, int vmin, int vmax);
  int           get();
  AttributeType get_type()
  {
    return AttributeType::INT_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  int value = 1;
  int vmin = 0;
  int vmax = 1;
};

class MapEnumAttribute : public Attribute
{
public:
  MapEnumAttribute() = default;
  MapEnumAttribute(std::map<std::string, int> value);
  int                        get();
  std::map<std::string, int> get_map();
  void                       set(std::string new_choice);
  AttributeType              get_type()
  {
    return AttributeType::MAP_ENUM_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  std::map<std::string, int> value = {};
  std::string                choice = "";
};

class MatrixAttribute : public Attribute
{
public:
  MatrixAttribute();
  std::vector<std::vector<float>> get();
  AttributeType                   get_type()
  {
    return AttributeType::MATRIX_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  std::vector<std::vector<float>> value = {{0.f, 0.f, 0.f},
                                           {0.f, 0.f, 0.f},
                                           {0.f, 0.f, 0.f}};
};

class PathAttribute : public Attribute
{
public:
  PathAttribute() = default;
  hmap::Path    get();
  AttributeType get_type()
  {
    return AttributeType::PATH_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  hmap::Path value;
};

class RangeAttribute : public Attribute
{
public:
  RangeAttribute() = default;
  RangeAttribute(hmap::Vec2<float> value);
  RangeAttribute(bool activate);
  hmap::Vec2<float> get();
  bool              is_activated()
  {
    return this->activate;
  };
  AttributeType get_type()
  {
    return AttributeType::RANGE_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  hmap::Vec2<float> value = {0.f, 1.f};
  bool              activate = true;
};

class SeedAttribute : public Attribute
{
public:
  SeedAttribute() = default;
  SeedAttribute(int value);
  int           get();
  AttributeType get_type()
  {
    return AttributeType::SEED_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  int value = 1;
};

class ShapeAttribute : public Attribute
{
public:
  ShapeAttribute();
  hmap::Vec2<int> get();
  void            set_value_max(hmap::Vec2<int> new_value_max);
  AttributeType   get_type()
  {
    return AttributeType::SHAPE_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  hmap::Vec2<int> value = {128, 128};
  hmap::Vec2<int> value_max = {0, 0};
};

class VecFloatAttribute : public Attribute
{
public:
  VecFloatAttribute();
  VecFloatAttribute(std::vector<float> value);
  std::vector<float> get();
  AttributeType      get_type()
  {
    return AttributeType::VEC_FLOAT_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  std::vector<float> value = {};
};

class VecIntAttribute : public Attribute
{
public:
  VecIntAttribute();
  VecIntAttribute(std::vector<int> value);
  std::vector<int> get();
  AttributeType    get_type()
  {
    return AttributeType::VEC_INT_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  std::vector<int> value = {};
};

class WaveNbAttribute : public Attribute
{
public:
  WaveNbAttribute() = default;
  WaveNbAttribute(hmap::Vec2<float> value);
  hmap::Vec2<float> get();
  AttributeType     get_type()
  {
    return AttributeType::WAVE_NB_ATTRIBUTE;
  }
  virtual bool render_settings(std::string label);

  SERIALIZATION_V2_IMPLEMENT_BASE();

  hmap::Vec2<float> value = {2.f, 2.f};
  bool              link_xy = true; // GUI
};

class AttributeInstancing
{
public:
  static std::string                get_name_from_type(AttributeType type);
  static AttributeType              get_type_from_name(std::string name);
  static std::unique_ptr<Attribute> create_attribute_from_type(
      AttributeType type);
  //     std::pair<succeeded, data>
  static std::pair<bool, nlohmann::json> quick_and_dirty_serialize_node_attribute(std::string key, Attribute* value);
  static std::pair<std::string, std::unique_ptr<Attribute>> quick_and_dirty_deserialize_node_attribute(nlohmann::json data); 
};

} // namespace hesiod