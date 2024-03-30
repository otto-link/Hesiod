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

#include <QJsonObject>

#include "highmap/geometry.hpp"
#include "highmap/vector.hpp"
#include "macrologger.h"

#include "hesiod/model/model_utils.hpp"

// clang-format off
#define NEW_ATTR_BOOL(...) std::make_unique<hesiod::BoolAttribute>(__VA_ARGS__)
#define NEW_ATTR_CLOUD(...) std::make_unique<hesiod::CloudAttribute>(__VA_ARGS__)
#define NEW_ATTR_COLOR(...) std::make_unique<hesiod::ColorAttribute>(__VA_ARGS__)
#define NEW_ATTR_FILENAME(...) std::make_unique<hesiod::FilenameAttribute>(__VA_ARGS__)
#define NEW_ATTR_FLOAT(...) std::make_unique<hesiod::FloatAttribute>(__VA_ARGS__)
#define NEW_ATTR_INT(...) std::make_unique<hesiod::IntAttribute>(__VA_ARGS__)
#define NEW_ATTR_MAPENUM(...) std::make_unique<hesiod::MapEnumAttribute>(__VA_ARGS__)
#define NEW_ATTR_MATRIX(...) std::make_unique<hesiod::MatrixAttribute>(__VA_ARGS__)
#define NEW_ATTR_PATH(...) std::make_unique<hesiod::PathAttribute>(__VA_ARGS__)
#define NEW_ATTR_RANGE(...) std::make_unique<hesiod::RangeAttribute>(__VA_ARGS__)
#define NEW_ATTR_SEED(...) std::make_unique<hesiod::SeedAttribute>(__VA_ARGS__)
#define NEW_ATTR_SHAPE(...) std::make_unique<hesiod::ShapeAttribute>(__VA_ARGS__)
#define NEW_ATTR_VECFLOAT(...) std::make_unique<hesiod::VecFloatAttribute>(__VA_ARGS__)
#define NEW_ATTR_VECINT(...) std::make_unique<hesiod::VecIntAttribute>(__VA_ARGS__)
#define NEW_ATTR_WAVENB(...) std::make_unique<hesiod::WaveNbAttribute>(__VA_ARGS__)

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

#define HSD_DEFAULT_KW 2.f
#define HSD_DEFAULT_SEED 1
#define HSD_FILENAME_DISPLAY_MAX_SIZE 32

namespace hesiod
{

enum class AttributeType
{
  INVALID,
  BOOL,
  CLOUD,
  COLOR,
  FILENAME,
  FLOAT,
  INT,
  MAP_ENUM,
  MATRIX,
  PATH,
  RANGE,
  SEED,
  SHAPE,
  VEC_FLOAT,
  VEC_INT,
  WAVE_NB
};

class Attribute
{
public:
  Attribute() = default;

  virtual AttributeType get_type() { return AttributeType::INVALID; }

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

  virtual void log_debug()
  {
    // LOG_DEBUG("address: %s", std::to_string((unsigned long long)(void
    // **)this).c_str());
  }

  virtual QJsonObject save() const
  {
    LOG_ERROR("TODO TODO TODO TODO TODO");
    return QJsonObject();
  }

  virtual void load(QJsonObject const & /* p */)
  {
    LOG_ERROR("TODO TODO TODO TODO TODO");
  }
};

// --- Derived

class BoolAttribute : public Attribute // OK JSON GUI
{
public:
  BoolAttribute() = default;
  BoolAttribute(bool value, std::string label);
  bool          get();
  AttributeType get_type() { return AttributeType::BOOL; }

  QJsonObject save() const override
  {
    QJsonObject model_json;
    model_json["value"] = QString(this->value ? "true" : "false");
    return model_json;
  }

  void load(QJsonObject const &p) override
  {
    bool ret = true;
    ret &= convert_qjsonvalue_to_bool(p["value"], this->value);

    if (!ret)
      LOG_ERROR("serialization in with BoolAttribute");
  }

  bool        value = true;
  std::string label = "";
};

class CloudAttribute : public Attribute // --------- TODO
{
public:
  CloudAttribute() = default;

  hmap::Cloud get();

  AttributeType get_type() { return AttributeType::CLOUD; }

  hmap::Cloud value;
};

class ColorAttribute : public Attribute // --------- TODO
{
public:
  ColorAttribute() = default;

  std::vector<float> get();

  AttributeType get_type() { return AttributeType::COLOR; }

  std::vector<float> value = {1.f, 1.f, 1.f};
};

class FilenameAttribute : public Attribute // OK JSON GUI
{
public:
  FilenameAttribute() = default;
  FilenameAttribute(std::string value, std::string filter = "", std::string label = "");
  std::string   get();
  AttributeType get_type() { return AttributeType::FILENAME; }

  QJsonObject save() const override
  {
    QJsonObject model_json;
    model_json["value"] = QString::fromStdString(this->value);
    model_json["filter"] = QString::fromStdString(this->filter);
    model_json["label"] = QString::fromStdString(this->label);
    return model_json;
  }

  void load(QJsonObject const &p) override
  {
    bool ret = true;
    ret &= convert_qjsonvalue_to_string(p["value"], this->value);
    ret &= convert_qjsonvalue_to_string(p["filter"], this->filter);
    ret &= convert_qjsonvalue_to_string(p["label"], this->label);

    if (!ret)
      LOG_ERROR("serialization in with FilenameAttribute");
  }

  std::string value = "";
  // filter eg: "Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
  std::string filter = "";
  std::string label = "";
};

class FloatAttribute : public Attribute // OK JSON GUI
{
public:
  FloatAttribute() = default;
  FloatAttribute(float value, float vmin, float vmax, std::string fmt = "%.2f");
  float         get();
  void          set(float new_value);
  AttributeType get_type() { return AttributeType::FLOAT; }

  QJsonObject save() const override
  {
    QJsonObject model_json;
    model_json["value"] = QString::number(this->value);
    model_json["vmin"] = QString::number(this->vmin);
    model_json["vmax"] = QString::number(this->vmax);
    model_json["fmt"] = QString::fromStdString(this->fmt);
    return model_json;
  }

  void load(QJsonObject const &p) override
  {
    bool ret = true;
    ret &= convert_qjsonvalue_to_float(p["value"], this->value);
    ret &= convert_qjsonvalue_to_float(p["vmin"], this->vmin);
    ret &= convert_qjsonvalue_to_float(p["vmax"], this->vmax);
    ret &= convert_qjsonvalue_to_string(p["fmt"], this->fmt);

    if (!ret)
      LOG_ERROR("serialization in with WaveNbAttribute");
  }

  float       value = 1.f;
  float       vmin = 0.f;
  float       vmax = 1.f;
  std::string fmt = "";
  bool        activate = true;
};

class IntAttribute : public Attribute // OK JSON GUI
{
public:
  IntAttribute() = default;
  IntAttribute(int value, int vmin, int vmax);
  int           get();
  AttributeType get_type() { return AttributeType::INT; }

  QJsonObject save() const override
  {
    QJsonObject model_json;
    model_json["value"] = QString::number(this->value);
    model_json["vmin"] = QString::number(this->vmin);
    model_json["vmax"] = QString::number(this->vmax);
    return model_json;
  }

  void load(QJsonObject const &p) override
  {
    bool ret = true;
    ret &= convert_qjsonvalue_to_int(p["value"], this->value);
    ret &= convert_qjsonvalue_to_int(p["vmin"], this->vmin);
    ret &= convert_qjsonvalue_to_int(p["vmax"], this->vmax);

    if (!ret)
      LOG_ERROR("serialization in with WaveNbAttribute");
  }
  int value = 1;
  int vmin = 0;
  int vmax = 1;
};

class MapEnumAttribute : public Attribute // OK JSON GUI
{
public:
  MapEnumAttribute() = default;
  MapEnumAttribute(std::map<std::string, int> value);
  int                        get();
  std::map<std::string, int> get_map();
  void                       set(std::string new_choice);
  AttributeType              get_type() { return AttributeType::MAP_ENUM; }

  QJsonObject save() const override
  {
    QJsonObject model_json;
    model_json["choice"] = QString::fromStdString(this->choice);
    return model_json;
  }

  void load(QJsonObject const &p) override
  {
    bool ret = true;
    ret &= convert_qjsonvalue_to_string(p["choice"], this->choice);

    if (!ret)
      LOG_ERROR("serialization error with MapEnumAttribute");
  }

  std::map<std::string, int> value = {};
  std::string                choice = "";
};

class MatrixAttribute : public Attribute // --------- TODO
{
public:
  MatrixAttribute();
  std::vector<std::vector<float>> get();
  AttributeType                   get_type() { return AttributeType::MATRIX; }

  std::vector<std::vector<float>> value = {{0.f, 0.f, 0.f},
                                           {0.f, 0.f, 0.f},
                                           {0.f, 0.f, 0.f}};
};

class PathAttribute : public Attribute // --------- TODO
{
public:
  PathAttribute() = default;
  hmap::Path    get();
  AttributeType get_type() { return AttributeType::PATH; }

  hmap::Path value;
};

class RangeAttribute : public Attribute // OK JSON GUI
{
public:
  RangeAttribute() = default;
  RangeAttribute(hmap::Vec2<float> value, bool activate, std::string fmt = "%.1f");
  RangeAttribute(hmap::Vec2<float> value, std::string fmt = "%.1f");
  RangeAttribute(bool activate, std::string fmt = "%.1f");
  hmap::Vec2<float> get();
  bool              is_activated() { return this->activate; };
  AttributeType     get_type() { return AttributeType::RANGE; }

  QJsonObject save() const override
  {
    QJsonObject model_json;
    model_json["value.x"] = QString::number(this->value.x);
    model_json["value.y"] = QString::number(this->value.y);
    model_json["activate"] = QString(this->activate ? "true" : "false");
    model_json["fmt"] = QString::fromStdString(fmt);
    return model_json;
  }

  void load(QJsonObject const &p) override
  {
    bool ret = true;
    ret &= convert_qjsonvalue_to_float(p["value.x"], this->value.x);
    ret &= convert_qjsonvalue_to_float(p["value.y"], this->value.y);
    ret &= convert_qjsonvalue_to_bool(p["activate"], this->activate);
    ret &= convert_qjsonvalue_to_string(p["fmt"], this->fmt);

    if (!ret)
      LOG_ERROR("serialization in with WaveNbAttribute");
  }

  hmap::Vec2<float> value = {0.f, 1.f};
  bool              activate = true;
  std::string       fmt = "%.1f";
};

class SeedAttribute : public Attribute // OK JSON GUI
{
public:
  SeedAttribute() = default;
  SeedAttribute(uint value);
  uint          get();
  AttributeType get_type() { return AttributeType::SEED; }

  void log_debug() override
  {
    Attribute::log_debug();
    LOG_DEBUG("seed / value: %d", this->value);
  }

  QJsonObject save() const override
  {
    QJsonObject model_json;
    model_json["value"] = QString::number(this->value);
    return model_json;
  }

  void load(QJsonObject const &p) override
  {
    bool ret = true;
    ret &= convert_qjsonvalue_to_uint(p["value"], this->value);

    LOG_DEBUG("seed in object: %d", this->value);

    if (!ret)
      LOG_ERROR("serialization error with SeedAttribute");
  }

  uint value = HSD_DEFAULT_SEED;
};

class ShapeAttribute : public Attribute
{
public:
  ShapeAttribute();
  hmap::Vec2<int> get();
  void            set_value_max(hmap::Vec2<int> new_value_max);
  AttributeType   get_type() { return AttributeType::SHAPE; }

  hmap::Vec2<int> value = {128, 128};
  hmap::Vec2<int> value_max = {0, 0};
};

class VecFloatAttribute : public Attribute
{
public:
  VecFloatAttribute();
  VecFloatAttribute(std::vector<float> value,
                    float              vmin,
                    float              vmax,
                    std::string        fmt = "%.2f");
  std::vector<float> get();
  AttributeType      get_type() { return AttributeType::VEC_FLOAT; }

  std::vector<float> value = {};
  float              vmin = 0.1f;
  float              vmax = 64.f;
  std::string        fmt = "%.2f";
};

class VecIntAttribute : public Attribute
{
public:
  VecIntAttribute();
  VecIntAttribute(std::vector<int> value);
  std::vector<int> get();
  AttributeType    get_type() { return AttributeType::VEC_INT; }

  std::vector<int> value = {};
};

class WaveNbAttribute : public Attribute // OK JSON GUI
{
public:
  WaveNbAttribute() = default;
  WaveNbAttribute(hmap::Vec2<float> value, std::string fmt = "%.1f");
  WaveNbAttribute(hmap::Vec2<float> value,
                  float             vmin,
                  float             vmax,
                  std::string       fmt = "%.1f");
  hmap::Vec2<float> get();
  AttributeType     get_type() { return AttributeType::WAVE_NB; }

  QJsonObject save() const override
  {
    QJsonObject model_json;
    model_json["value.x"] = QString::number(this->value.x);
    model_json["value.y"] = QString::number(this->value.y);
    model_json["link_xy"] = QString(this->link_xy ? "true" : "false");
    model_json["vmin"] = QString::number(this->vmin);
    model_json["vmax"] = QString::number(this->vmax);
    model_json["fmt"] = QString::fromStdString(this->fmt);
    return model_json;
  }

  void load(QJsonObject const &p) override
  {
    bool ret = true;
    ret &= convert_qjsonvalue_to_float(p["value.x"], this->value.x);
    ret &= convert_qjsonvalue_to_float(p["value.y"], this->value.y);
    ret &= convert_qjsonvalue_to_bool(p["link_xy"], this->link_xy);
    ret &= convert_qjsonvalue_to_float(p["vmin"], this->vmin);
    ret &= convert_qjsonvalue_to_float(p["vmax"], this->vmax);
    ret &= convert_qjsonvalue_to_string(p["fmt"], this->fmt);

    if (!ret)
      LOG_ERROR("serialization in with WaveNbAttribute");
  }

  hmap::Vec2<float> value = {HSD_DEFAULT_KW, HSD_DEFAULT_KW};

  // GUI
  bool        link_xy = true;
  float       vmin = 0.1f;
  float       vmax = 64.f;
  std::string fmt = "%.1f";
};

// std::string get_name_from_type(AttributeType type);
//
// AttributeType get_type_from_name(std::string name);

} // namespace hesiod
