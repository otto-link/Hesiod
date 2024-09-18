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

#include "highmap/algebra.hpp"
#include "nlohmann/json.hpp"

#include "hesiod/logger.hpp"

#define HSD_DEFAULT_KW 2.f
#define HSD_DEFAULT_SEED 1
#define HSD_FILENAME_DISPLAY_MAX_SIZE 16

namespace hesiod
{

enum class AttributeType : int
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

/**
 * @brief Creates a unique pointer to an attribute of the specified type.
 *
 * This function is a template that can create any type of attribute by forwarding
 * the provided arguments to the constructor of the attribute.
 *
 * @tparam AttributeType The type of the attribute to create. Must be a class that
 * inherits from an attribute base class.
 * @tparam Args Variadic template parameter that allows passing any number of arguments
 * to the constructor of the attribute.
 *
 * @param args The arguments to forward to the constructor of the attribute.
 * @return std::unique_ptr<AttributeType> A unique pointer to the newly created attribute.
 */
template <typename AttributeType, typename... Args>
std::unique_ptr<AttributeType> create_attr(Args &&...args)
{
  return std::make_unique<AttributeType>(std::forward<Args>(args)...);
}

class Attribute
{
public:
  Attribute() = default;

  Attribute(const std::string &label) : label(label) {}

  std::string get_label() const { return this->label; }

  virtual AttributeType get_type() const = 0;

  template <class T = void> T *get_ref()
  {
    T *ptr = dynamic_cast<T *>(this);
    if (ptr)
      return ptr;
    else
    {
      HSDLOG->critical("in Attribute, trying to get an attribute type which is not "
                       "compatible with the current instance. Get type is: [{}]",
                       typeid(T).name());
      throw std::runtime_error("wrong type");
    }
  }

  virtual void json_from(nlohmann::json const &);

  virtual nlohmann::json json_to() const;

  void set_label(const std::string &new_label) { this->label = new_label; }

protected:
  std::string label;
};

// --- Derived

class BoolAttribute : public Attribute
{
public:
  BoolAttribute() = default;
  BoolAttribute(bool value, std::string label = "");
  bool          get();
  AttributeType get_type() const { return AttributeType::BOOL; }

  void           json_from(nlohmann::json const &json) override;
  nlohmann::json json_to() const override;

  bool value = true;
};

class ColorAttribute : public Attribute
{
public:
  ColorAttribute() = default;

  std::vector<float> get();

  AttributeType get_type() const { return AttributeType::COLOR; }

  void json_from(nlohmann::json const &json) override;

  nlohmann::json json_to() const override;

  std::vector<float> value = {1.f, 1.f, 1.f, 1.f};
};

class ColorGradientAttribute : public Attribute
{
public:
  ColorGradientAttribute();
  ColorGradientAttribute(std::vector<std::vector<float>> value);
  std::vector<std::vector<float>> get();
  AttributeType get_type() const { return AttributeType::COLOR_GRADIENT; }
  void          json_from(nlohmann::json const &json) override;

  nlohmann::json json_to() const override;

  std::vector<std::vector<float>> value = {};
};

class FilenameAttribute : public Attribute
{
public:
  FilenameAttribute() = default;
  FilenameAttribute(std::string value, std::string filter = "", std::string label = "");
  std::string    get();
  AttributeType  get_type() const { return AttributeType::FILENAME; }
  void           json_from(nlohmann::json const &json) override;
  nlohmann::json json_to() const override;

  std::string value = "";
  // filter eg: "Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
  std::string filter = "";
};

class FloatAttribute : public Attribute
{
public:
  FloatAttribute() = default;
  FloatAttribute(float value, float vmin, float vmax, std::string fmt = "%.2f");
  float          get();
  void           set(float new_value);
  AttributeType  get_type() const { return AttributeType::FLOAT; }
  void           json_from(nlohmann::json const &json) override;
  nlohmann::json json_to() const override;

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
  AttributeType get_type() const { return AttributeType::INT; }

  /**
   * @brief Deserializes the attribute from a JSON object.
   *
   * Updates the value, vmin, and vmax of the attribute based on the JSON input.
   *
   * @param json The JSON object containing the attribute data.
   */
  void json_from(nlohmann::json const &json) override;

  /**
   * @brief Serializes the attribute to a JSON object.
   *
   * @return A JSON object representing the attribute.
   */
  nlohmann::json json_to() const override;

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
  AttributeType              get_type() const { return AttributeType::MAP_ENUM; }

  void json_from(nlohmann::json const &json) override;

  nlohmann::json json_to() const override;

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
  AttributeType     get_type() const { return AttributeType::RANGE; }
  void              json_from(nlohmann::json const &json) override;
  nlohmann::json    json_to() const override;

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
  uint           get();
  AttributeType  get_type() const { return AttributeType::SEED; }
  void           json_from(nlohmann::json const &json) override;
  nlohmann::json json_to() const override;

  uint value = HSD_DEFAULT_SEED;
};

class StringAttribute : public Attribute
{
public:
  StringAttribute() = default;
  StringAttribute(std::string value);
  std::string    get();
  AttributeType  get_type() const { return AttributeType::STRING; }
  void           json_from(nlohmann::json const &json) override;
  nlohmann::json json_to() const override;

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
  AttributeType      get_type() const { return AttributeType::VEC_FLOAT; }
  void               json_from(nlohmann::json const &json) override;
  nlohmann::json     json_to() const override;

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
  AttributeType    get_type() const { return AttributeType::VEC_INT; }
  void             json_from(nlohmann::json const &json) override;
  nlohmann::json   json_to() const override;

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
  AttributeType     get_type() const { return AttributeType::WAVE_NB; }
  void              json_from(nlohmann::json const &json) override;
  nlohmann::json    json_to() const override;

  hmap::Vec2<float> value = {HSD_DEFAULT_KW, HSD_DEFAULT_KW};

  // GUI
  bool        link_xy = true;
  float       vmin = 0.1f;
  float       vmax = 64.f;
  std::string fmt = "%.1f";
};

} // namespace hesiod
