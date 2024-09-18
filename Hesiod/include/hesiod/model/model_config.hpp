/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include "highmap/algebra.hpp"
#include "nlohmann/json.hpp"

namespace hesiod
{

/**
 * @struct ModelConfig
 * @brief Model configuration class that holds the base parameters of the node graph
 * model.
 *
 * This class represents the configuration for a model, including parameters such as
 * the heightmap shape, tiling for distributed computation, and tile overlap. It also
 * provides methods for logging the configuration details and serializing/deserializing
 * to/from JSON.
 */
struct ModelConfig
{
  /**
   * @brief Heightmap shape, defining the width and height.
   *
   * This parameter specifies the dimensions of the heightmap as a vector of integers.
   * By default, the shape is set to {1024, 1024}.
   */
  hmap::Vec2<int> shape = {1024, 1024};

  /**
   * @brief Heightmap tiling for distributed computation.
   *
   * This parameter defines how the heightmap is divided into tiles for distributed
   * processing, with the default tiling set to {4, 4}.
   */
  hmap::Vec2<int> tiling = {4, 4};

  /**
   * @brief Tile overlap ratio in the range [0, 1].
   *
   * Specifies the overlap between tiles as a floating-point value. The default
   * overlap is 0.25, meaning there is 25% overlap between adjacent tiles.
   */
  float overlap = 0.25f;

  /**
   * @brief Logs debug information about the current model configuration.
   *
   * This method outputs relevant configuration details such as the heightmap shape,
   * tiling, and overlap for debugging purposes.
   */
  void log_debug() const;

  /**
   * @brief Deserializes the model configuration from a JSON object.
   *
   * This method updates the model configuration based on the provided JSON input,
   * setting the shape, tiling, and overlap accordingly.
   *
   * @param json A JSON object containing the configuration data.
   */
  void json_from(nlohmann::json const &json);

  /**
   * @brief Serializes the model configuration to a JSON object.
   *
   * This method converts the current model configuration into a JSON format,
   * including the shape, tiling, and overlap.
   *
   * @return nlohmann::json A JSON object representing the model configuration.
   */
  nlohmann::json json_to() const;

  /**
   * @brief Sets a new shape for the object.
   *
   * This function updates the internal shape of the object to the specified value.
   *
   * @param new_shape A 2D vector representing the new shape dimensions (e.g., width and
   * height).
   */
  void set_shape(const hmap::Vec2<int> &new_shape) { this->shape = new_shape; }
};

} // namespace hesiod
