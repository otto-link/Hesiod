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
#include "serialization.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

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

class Attribute : public serialization::SerializationBase
{
public:
  Attribute() = default;
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

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &)
  {
    // empty
  }
#endif
};

// --- Derived

class BoolAttribute : public Attribute
{
public:
  BoolAttribute() = default;
  BoolAttribute(bool value);
  bool         get();
  virtual bool render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value", this->value));
  }
#endif

  SERIALIZATION_V2_IMPLEMENT_BASE();

  bool value = true;
};

class CloudAttribute : public Attribute
{
public:
  CloudAttribute() = default;
  hmap::Cloud  get();
  virtual bool render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    std::vector<float> x = this->value.get_x();
    std::vector<float> y = this->value.get_y();
    std::vector<float> v = this->value.get_values();

    ar(cereal::make_nvp("x", x),
       cereal::make_nvp("y", y),
       cereal::make_nvp("v", v));

    this->value = hmap::Cloud(x, y, v);
  }
#endif

  SERIALIZATION_V2_IMPLEMENT_BASE();

  hmap::Cloud value;
};

class ColorAttribute : public Attribute
{
public:
  ColorAttribute() = default;
  std::vector<float> get();
  virtual bool       render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value", value));
  }
#endif

  SERIALIZATION_V2_IMPLEMENT_BASE();

  std::vector<float> value = {1.f, 1.f, 1.f};
};

class FilenameAttribute : public Attribute
{
public:
  FilenameAttribute() = default;
  FilenameAttribute(std::string value);
  std::string  get();
  virtual bool render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value", this->value));
  }
#endif

  SERIALIZATION_V2_IMPLEMENT_BASE();

  std::string value = "";
};

class FloatAttribute : public Attribute
{
public:
  FloatAttribute() = default;
  FloatAttribute(float value, float vmin, float vmax);
  float        get();
  void         set(float new_value);
  virtual bool render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value", this->value),
       cereal::make_nvp("vmin", this->vmin),
       cereal::make_nvp("vmax", this->vmax));
  }
#endif

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
  int          get();
  virtual bool render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value", this->value),
       cereal::make_nvp("vmin", this->vmin),
       cereal::make_nvp("vmax", this->vmax));
  }
#endif

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
  virtual bool               render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value", this->value),
       cereal::make_nvp("choice", this->choice));
  }
#endif

  SERIALIZATION_V2_IMPLEMENT_BASE();

  std::map<std::string, int> value = {};
  std::string                choice = "";
};

class MatrixAttribute : public Attribute
{
public:
  MatrixAttribute();
  std::vector<std::vector<float>> get();
  virtual bool                    render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value", this->value));
  }
#endif

  SERIALIZATION_V2_IMPLEMENT_BASE();

  std::vector<std::vector<float>> value = {{0.f, 0.f, 0.f},
                                           {0.f, 0.f, 0.f},
                                           {0.f, 0.f, 0.f}};
};

class PathAttribute : public Attribute
{
public:
  PathAttribute() = default;
  hmap::Path   get();
  virtual bool render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    std::vector<float> x = this->value.get_x();
    std::vector<float> y = this->value.get_y();
    std::vector<float> v = this->value.get_values();
    bool               closed = this->value.closed;

    ar(cereal::make_nvp("x", x),
       cereal::make_nvp("y", y),
       cereal::make_nvp("v", v),
       cereal::make_nvp("closed", closed));

    this->value = hmap::Path(x, y, v);
    this->value.closed = closed;
  }
#endif

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
  virtual bool render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value.x", this->value.x),
       cereal::make_nvp("value.y", this->value.y),
       cereal::make_nvp("activate", this->activate));
  }
#endif

  hmap::Vec2<float> value = {0.f, 1.f};
  bool              activate = true;
};

class SeedAttribute : public Attribute
{
public:
  SeedAttribute() = default;
  SeedAttribute(int value);
  int          get();
  virtual bool render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value", this->value));
  }
#endif

  int value = 1;
};

class ShapeAttribute : public Attribute
{
public:
  ShapeAttribute();
  hmap::Vec2<int> get();
  void            set_value_max(hmap::Vec2<int> new_value_max);
  virtual bool    render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value.x", this->value.x),
       cereal::make_nvp("value.y", this->value.y));
  }
#endif

  hmap::Vec2<int> value = {128, 128};
  hmap::Vec2<int> value_max = {0, 0};
};

class VecFloatAttribute : public Attribute
{
public:
  VecFloatAttribute();
  VecFloatAttribute(std::vector<float> value);
  std::vector<float> get();
  virtual bool       render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value", this->value));
  }
#endif

  std::vector<float> value = {};
};

class VecIntAttribute : public Attribute
{
public:
  VecIntAttribute();
  VecIntAttribute(std::vector<int> value);
  std::vector<int> get();
  virtual bool     render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value", this->value));
  }
#endif

  std::vector<int> value = {};
};

class WaveNbAttribute : public Attribute
{
public:
  WaveNbAttribute() = default;
  WaveNbAttribute(hmap::Vec2<float> value);
  hmap::Vec2<float> get();
  virtual bool      render_settings(std::string label);

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    Attribute::serialize<Archive>(ar);
    ar(cereal::make_nvp("value.x", this->value.x),
       cereal::make_nvp("value.y", this->value.y),
       cereal::make_nvp("link_xy", this->link_xy));
  }
#endif

  hmap::Vec2<float> value = {2.f, 2.f};
  bool              link_xy = true; // GUI
};

} // namespace hesiod

// clang-format off
#ifdef USE_CEREAL
CEREAL_REGISTER_TYPE(hesiod::BoolAttribute);
CEREAL_REGISTER_TYPE(hesiod::CloudAttribute);
CEREAL_REGISTER_TYPE(hesiod::ColorAttribute);
CEREAL_REGISTER_TYPE(hesiod::FilenameAttribute);
CEREAL_REGISTER_TYPE(hesiod::FloatAttribute);
CEREAL_REGISTER_TYPE(hesiod::IntAttribute);
CEREAL_REGISTER_TYPE(hesiod::MapEnumAttribute);
CEREAL_REGISTER_TYPE(hesiod::MatrixAttribute);
CEREAL_REGISTER_TYPE(hesiod::PathAttribute);
CEREAL_REGISTER_TYPE(hesiod::RangeAttribute);
CEREAL_REGISTER_TYPE(hesiod::SeedAttribute);
CEREAL_REGISTER_TYPE(hesiod::ShapeAttribute);
CEREAL_REGISTER_TYPE(hesiod::VecFloatAttribute);
CEREAL_REGISTER_TYPE(hesiod::VecIntAttribute);
CEREAL_REGISTER_TYPE(hesiod::WaveNbAttribute);

CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::BoolAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::CloudAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::ColorAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::FilenameAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::FloatAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::IntAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::MatrixAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::MapEnumAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::PathAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::RangeAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::SeedAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::ShapeAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::VecFloatAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::VecIntAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::WaveNbAttribute);
#endif
// clang-format on