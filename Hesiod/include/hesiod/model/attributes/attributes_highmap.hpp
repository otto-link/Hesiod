/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file attribute_highmap.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Defines attributes for handling highmap geometries like Cloud and Path.
 *
 * This file contains the definitions of the `CloudAttribute` and `PathAttribute`
 * classes, which are specialized `Attribute` types for storing and handling
 * HighMap geometric data such as clouds and paths. These classes provide
 * serialization to and from JSON, as well as type-specific accessors.
 *
 * The file makes use of the nlohmann::json library for JSON handling and
 * imports from the highmap library for geometric data structures.
 *
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023 Otto Link
 *
 */

#pragma once
#include <memory>

#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"
#include "nlohmann/json.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/attributes/attributes.hpp"

namespace hesiod
{

/**
 * @class CloudAttribute
 * @brief Attribute class for managing highmap cloud geometry data.
 *
 * This class represents a specialized attribute for storing and managing
 * highmap cloud geometry. It provides methods to get the cloud value,
 * serialize the data to JSON, and deserialize it from JSON.
 */
class CloudAttribute : public Attribute
{
public:
  /**
   * @brief Default constructor for CloudAttribute.
   */
  CloudAttribute() = default;

  /**
   * @brief Gets the current cloud geometry stored in the attribute.
   *
   * @return hmap::Cloud The cloud geometry.
   */
  hmap::Cloud get();

  /**
   * @brief Returns the type of the attribute.
   *
   * Overrides the base class method to return AttributeType::CLOUD,
   * indicating that this is a cloud attribute.
   *
   * @return AttributeType The attribute type (AttributeType::CLOUD).
   */
  AttributeType get_type() { return AttributeType::CLOUD; }

  /**
   * @brief Serializes the attribute to a JSON object.
   *
   * @return nlohmann::json A JSON object representing the attribute.
   */
  nlohmann::json json_to() const override;

  /**
   * @brief Deserializes the attribute from a JSON object.
   *
   * Updates the cloud geometry based on the JSON input.
   *
   * @param json The JSON object containing the attribute data.
   */
  void json_from(nlohmann::json const &json) override;

  /// The cloud geometry data stored in the attribute.
  hmap::Cloud value;
};

/**
 * @class PathAttribute
 * @brief Attribute class for managing highmap path geometry data.
 *
 * This class represents a specialized attribute for storing and managing
 * highmap path geometry. It provides methods to get the path value,
 * serialize the data to JSON, and deserialize it from JSON.
 */
class PathAttribute : public Attribute
{
public:
  /**
   * @brief Default constructor for PathAttribute.
   */
  PathAttribute() = default;

  /**
   * @brief Gets the current path geometry stored in the attribute.
   *
   * @return hmap::Path The path geometry.
   */
  hmap::Path get();

  /**
   * @brief Returns the type of the attribute.
   *
   * Overrides the base class method to return AttributeType::PATH,
   * indicating that this is a path attribute.
   *
   * @return AttributeType The attribute type (AttributeType::PATH).
   */
  AttributeType get_type() { return AttributeType::PATH; }

  /**
   * @brief Serializes the attribute to a JSON object.
   *
   * @return nlohmann::json A JSON object representing the attribute.
   */
  nlohmann::json json_to() const override;

  /**
   * @brief Deserializes the attribute from a JSON object.
   *
   * Updates the path geometry based on the JSON input.
   *
   * @param json The JSON object containing the attribute data.
   */
  void json_from(nlohmann::json const &json) override;

  /// The path geometry data stored in the attribute.
  hmap::Path value;
};

} // namespace hesiod
