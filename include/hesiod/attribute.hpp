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
#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

// clang-format off
#define NEW_ATTR_BOOL(a) std::make_unique<hesiod::BoolAttribute>(a)
#define NEW_ATTR_FLOAT(a, b, c) std::make_unique<hesiod::FloatAttribute>(a, b, c)
#define NEW_ATTR_INT(a, b, c) std::make_unique<hesiod::IntAttribute>(a, b, c)
#define NEW_ATTR_MAPENUM(a) std::make_unique<hesiod::MapEnumAttribute>(a)
#define NEW_ATTR_MATRIX() std::make_unique<hesiod::MatrixAttribute>()
#define NEW_ATTR_RANGE(a) std::make_unique<hesiod::RangeAttribute>(a)
#define NEW_ATTR_SEED(a) std::make_unique<hesiod::SeedAttribute>(a)
#define NEW_ATTR_WAVENB(a) std::make_unique<hesiod::WaveNbAttribute>(a)

#define GET_ATTR_BOOL(s) this->attr.at(s)->get_ref<BoolAttribute>()->get()
#define GET_ATTR_FLOAT(s) this->attr.at(s)->get_ref<FloatAttribute>()->get()
#define GET_ATTR_INT(s) this->attr.at(s)->get_ref<IntAttribute>()->get()
#define GET_ATTR_MAPENUM(s) this->attr.at(s)->get_ref<MapEnumAttribute>()->get()
#define GET_ATTR_MATRIX(s) this->attr.at(s)->get_ref<MatrixAttribute>()->get()
#define GET_ATTR_RANGE(s) this->attr.at(s)->get_ref<RangeAttribute>()->get()
#define GET_ATTR_SEED(s) this->attr.at(s)->get_ref<SeedAttribute>()->get()
#define GET_ATTR_WAVENB(s) this->attr.at(s)->get_ref<WaveNbAttribute>()->get()
// clang-format on

namespace hesiod
{

class Attribute
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

// base

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

private:
  bool value = true;
};

class FloatAttribute : public Attribute
{
public:
  FloatAttribute() = default;
  FloatAttribute(float value, float vmin, float vmax);
  float        get();
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

private:
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

private:
  int value = 1.f;
  int vmin = 0.f;
  int vmax = 1.f;
};

// class StringAttribute : public Attribute
// {
// public:
//   StringAttribute() = default;
//   StringAttribute(std::string value);
//   std::string          get();
//   virtual bool render_settings(std::string label);

// #ifdef USE_CEREAL
//   template <class Archive> void serialize(Archive &ar)
//   {
//     Attribute::serialize<Archive>(ar);
//     ar(cereal::make_nvp("value", this->value));
//   }
// #endif

// private:
//   std::string value = "";
// };

// specific

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

private:
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

private:
  std::vector<std::vector<float>> value = {{0.f, 0.f, 0.f},
                                           {0.f, 0.f, 0.f},
                                           {0.f, 0.f, 0.f}};
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

private:
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

private:
  int value = 1;
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

private:
  hmap::Vec2<float> value = {2.f, 2.f};
  bool              link_xy = true; // GUI
};

} // namespace hesiod

CEREAL_REGISTER_TYPE(hesiod::BoolAttribute);
CEREAL_REGISTER_TYPE(hesiod::FloatAttribute);
CEREAL_REGISTER_TYPE(hesiod::IntAttribute);
CEREAL_REGISTER_TYPE(hesiod::MapEnumAttribute);
CEREAL_REGISTER_TYPE(hesiod::MatrixAttribute);
CEREAL_REGISTER_TYPE(hesiod::RangeAttribute);
CEREAL_REGISTER_TYPE(hesiod::SeedAttribute);
// CEREAL_REGISTER_TYPE(hesiod::StringAttribute);
CEREAL_REGISTER_TYPE(hesiod::WaveNbAttribute);

CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::BoolAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::FloatAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::IntAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute,
                                     hesiod::MatrixAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute,
                                     hesiod::MapEnumAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::RangeAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute, hesiod::SeedAttribute);
// CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute,
// hesiod::StringAttribute);
CEREAL_REGISTER_POLYMORPHIC_RELATION(hesiod::Attribute,
                                     hesiod::WaveNbAttribute);
