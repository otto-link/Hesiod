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

#include "nlohmann/json.hpp"

#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"

#include "hesiod/logger.hpp"

// clang-format off
#define NEW_ATTR_BOOL(...) std::make_unique<hesiod::BoolAttribute>(__VA_ARGS__)
#define NEW_ATTR_COLOR(...) std::make_unique<hesiod::ColorAttribute>(__VA_ARGS__)
#define NEW_ATTR_COLORGRADIENT(...) std::make_unique<hesiod::ColorGradientAttribute>(__VA_ARGS__)
#define NEW_ATTR_FILENAME(...) std::make_unique<hesiod::FilenameAttribute>(__VA_ARGS__)
#define NEW_ATTR_FLOAT(...) std::make_unique<hesiod::FloatAttribute>(__VA_ARGS__)
#define NEW_ATTR_INT(...) std::make_unique<hesiod::IntAttribute>(__VA_ARGS__)
#define NEW_ATTR_MAPENUM(...) std::make_unique<hesiod::MapEnumAttribute>(__VA_ARGS__)
#define NEW_ATTR_MATRIX(...) std::make_unique<hesiod::MatrixAttribute>(__VA_ARGS__)
#define NEW_ATTR_RANGE(...) std::make_unique<hesiod::RangeAttribute>(__VA_ARGS__)
#define NEW_ATTR_SEED(...) std::make_unique<hesiod::SeedAttribute>(__VA_ARGS__)
#define NEW_ATTR_SHAPE(...) std::make_unique<hesiod::ShapeAttribute>(__VA_ARGS__)
#define NEW_ATTR_STRING(...) std::make_unique<hesiod::StringAttribute>(__VA_ARGS__)
#define NEW_ATTR_VECFLOAT(...) std::make_unique<hesiod::VecFloatAttribute>(__VA_ARGS__)
#define NEW_ATTR_VECINT(...) std::make_unique<hesiod::VecIntAttribute>(__VA_ARGS__)
#define NEW_ATTR_WAVENB(...) std::make_unique<hesiod::WaveNbAttribute>(__VA_ARGS__)

#define GET_ATTR_BOOL(s) this->attr.at(s)->get_ref<BoolAttribute>()->get()
#define GET_ATTR_COLOR(s) this->attr.at(s)->get_ref<ColorAttribute>()->get()
#define GET_ATTR_COLORGRADIENT(s) this->attr.at(s)->get_ref<ColorGradientAttribute>()->get()
#define GET_ATTR_FILENAME(s) this->attr.at(s)->get_ref<FilenameAttribute>()->get()
#define GET_ATTR_FLOAT(s) this->attr.at(s)->get_ref<FloatAttribute>()->get()
#define GET_ATTR_INT(s) this->attr.at(s)->get_ref<IntAttribute>()->get()
#define GET_ATTR_MAPENUM(s) this->attr.at(s)->get_ref<MapEnumAttribute>()->get()
#define GET_ATTR_MATRIX(s) this->attr.at(s)->get_ref<MatrixAttribute>()->get()
#define GET_ATTR_RANGE(s) this->attr.at(s)->get_ref<RangeAttribute>()->get()
#define GET_ATTR_SEED(s) this->attr.at(s)->get_ref<SeedAttribute>()->get()
#define GET_ATTR_SHAPE(s) this->attr.at(s)->get_ref<ShapeAttribute>()->get()
#define GET_ATTR_STRING(s) this->attr.at(s)->get_ref<StringAttribute>()->get()
#define GET_ATTR_VECFLOAT(s) this->attr.at(s)->get_ref<VecFloatAttribute>()->get()
#define GET_ATTR_VECINT(s) this->attr.at(s)->get_ref<VecIntAttribute>()->get()
#define GET_ATTR_WAVENB(s) this->attr.at(s)->get_ref<WaveNbAttribute>()->get()
// clang-format on

#define HSD_DEFAULT_KW 2.f
#define HSD_DEFAULT_SEED 1
#define HSD_FILENAME_DISPLAY_MAX_SIZE 16

namespace hesiod
{

enum class AttributeType
{
  INVALID,
  BOOL,
  CLOUD,
  COLOR,
  COLOR_GRADIENT,
  FILENAME,
  FLOAT,
  INT,
  MAP_ENUM,
  MATRIX,
  PATH,
  RANGE,
  SEED,
  SHAPE,
  STRING,
  VEC_FLOAT,
  VEC_INT,
  WAVE_NB
};

static std::map<hesiod::AttributeType, std::string> attribute_type_map = {
    {hesiod::AttributeType::BOOL, "Bool"},
    {hesiod::AttributeType::CLOUD, "HighMap Cloud Object"},
    {hesiod::AttributeType::COLOR, "Color"},
    {hesiod::AttributeType::COLOR_GRADIENT, "Color gradient"},
    {hesiod::AttributeType::FILENAME, "Filename"},
    {hesiod::AttributeType::FLOAT, "Float"},
    {hesiod::AttributeType::INT, "Integer"},
    {hesiod::AttributeType::MAP_ENUM, "Enumeration"},
    {hesiod::AttributeType::MATRIX, "Matrix"},
    {hesiod::AttributeType::PATH, "HighMap Path object"},
    {hesiod::AttributeType::RANGE, "Value range"},
    {hesiod::AttributeType::SEED, "Random seed"},
    {hesiod::AttributeType::SHAPE, "Shape"},
    {hesiod::AttributeType::STRING, "String"},
    {hesiod::AttributeType::VEC_FLOAT, "Float vector"},
    {hesiod::AttributeType::VEC_INT, "Integer vector"},
    {hesiod::AttributeType::WAVE_NB, "Wavenumber"}};

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
      SPDLOG->critical("in Attribute, trying to get an attribute type which is not "
                       "compatible with the current instance. Get type is: [{}]",
                       typeid(T).name());
      throw std::runtime_error("wrong type");
    }
  }

  virtual nlohmann::json json_to() const = 0;

  virtual void json_from(nlohmann::json const &) = 0;
};

// --- Derived

class BoolAttribute : public Attribute
{
public:
  BoolAttribute() = default;
  BoolAttribute(bool value, std::string label = "");
  bool          get();
  AttributeType get_type() { return AttributeType::BOOL; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  bool        value = true;
  std::string label = "";
};

class ColorAttribute : public Attribute
{
public:
  ColorAttribute() = default;

  std::vector<float> get();

  AttributeType get_type() { return AttributeType::COLOR; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  std::vector<float> value = {1.f, 1.f, 1.f, 1.f};
};

class ColorGradientAttribute : public Attribute
{
public:
  ColorGradientAttribute();
  ColorGradientAttribute(std::vector<std::vector<float>> value);
  std::vector<std::vector<float>> get();
  AttributeType                   get_type() { return AttributeType::COLOR_GRADIENT; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  std::vector<std::vector<float>> value = {};
};

class FilenameAttribute : public Attribute
{
public:
  FilenameAttribute() = default;
  FilenameAttribute(std::string value, std::string filter = "", std::string label = "");
  std::string   get();
  AttributeType get_type() { return AttributeType::FILENAME; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  std::string value = "";
  // filter eg: "Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
  std::string filter = "";
  std::string label = "";
};

class FloatAttribute : public Attribute
{
public:
  FloatAttribute() = default;
  FloatAttribute(float value, float vmin, float vmax, std::string fmt = "%.2f");
  float         get();
  void          set(float new_value);
  AttributeType get_type() { return AttributeType::FLOAT; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  float       value = 1.f;
  float       vmin = 0.f;
  float       vmax = 1.f;
  std::string fmt = "";
  bool        activate = true;
};

/**
 * @class IntAttribute
 * @brief A class that represents an integer attribute with minimum and maximum bounds.
 *
 * IntAttribute is derived from the Attribute class and represents an integer attribute
 * that can be serialized to and from JSON. It contains a value and enforces minimum
 * (vmin) and maximum (vmax) constraints.
 */
class IntAttribute : public Attribute
{
public:
  /**
   * @brief Default constructor for IntAttribute.
   *
   * Initializes the attribute with default values (value = 1, vmin = 0, vmax = 1).
   */
  IntAttribute() = default;

  /**
   * @brief Parameterized constructor for IntAttribute.
   *
   * Initializes the attribute with a specific value and its minimum and maximum bounds.
   *
   * @param value The initial value of the attribute.
   * @param vmin The minimum allowable value.
   * @param vmax The maximum allowable value.
   */
  IntAttribute(int value, int vmin, int vmax);

  /**
   * @brief Gets the current value of the attribute.
   *
   * @return The current integer value of the attribute.
   */
  int get();

  /**
   * @brief Returns the type of the attribute.
   *
   * Overrides the base class method to return AttributeType::INT,
   * indicating that this is an integer attribute.
   *
   * @return The attribute type (AttributeType::INT).
   */
  AttributeType get_type() { return AttributeType::INT; }

  /**
   * @brief Serializes the attribute to a JSON object.
   *
   * @return A JSON object representing the attribute.
   */
  nlohmann::json json_to() const override;

  /**
   * @brief Deserializes the attribute from a JSON object.
   *
   * Updates the value, vmin, and vmax of the attribute based on the JSON input.
   *
   * @param json The JSON object containing the attribute data.
   */
  void json_from(nlohmann::json const &json) override;

  /// The current integer value of the attribute.
  int value = 1;

  /// The minimum allowable value for the attribute.
  int vmin = 0;

  /// The maximum allowable value for the attribute.
  int vmax = 1;
};

class MapEnumAttribute : public Attribute
{
public:
  MapEnumAttribute() = default;
  MapEnumAttribute(std::map<std::string, int> value);
  MapEnumAttribute(std::map<std::string, int> value, std::string choice);
  int                        get();
  std::map<std::string, int> get_map();
  void                       set(std::string new_choice);
  AttributeType              get_type() { return AttributeType::MAP_ENUM; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  std::map<std::string, int> value = {};
  std::string                choice = "";
  bool                       is_colormap_enum = false;
};

class RangeAttribute : public Attribute
{
public:
  RangeAttribute() = default;
  RangeAttribute(hmap::Vec2<float> value, std::string fmt = "%.2f");
  RangeAttribute(hmap::Vec2<float> value,
                 float             vmin,
                 float             vmax,
                 std::string       fmt = "%.2f");
  RangeAttribute(std::string fmt);
  hmap::Vec2<float> get();
  AttributeType     get_type() { return AttributeType::RANGE; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  hmap::Vec2<float> value = {0.f, 1.f};
  float             vmin = -0.5f;
  float             vmax = 1.5f;
  std::string       fmt = "%.2f";
};

class SeedAttribute : public Attribute
{
public:
  SeedAttribute() = default;
  SeedAttribute(uint value);
  uint          get();
  AttributeType get_type() { return AttributeType::SEED; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  uint value = HSD_DEFAULT_SEED;
};

class StringAttribute : public Attribute
{
public:
  StringAttribute() = default;
  StringAttribute(std::string value);
  std::string   get();
  AttributeType get_type() { return AttributeType::STRING; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  std::string value = "";
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

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

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
  VecIntAttribute(std::vector<int> value, int vmin, int vmax);
  std::vector<int> get();
  AttributeType    get_type() { return AttributeType::VEC_INT; }

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

  std::vector<int> value = {};
  int              vmin = 0;
  int              vmax = 64;
};

class WaveNbAttribute : public Attribute
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

  nlohmann::json json_to() const override;

  void json_from(nlohmann::json const &json) override;

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
