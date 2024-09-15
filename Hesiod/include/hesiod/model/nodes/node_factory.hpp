/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file node_factory.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "gnode/node.hpp"

#include "hesiod/model/model_config.hpp"

/**
 * @brief Macro to generate a case statement for creating a node of the specified type.
 *
 * This macro is used in a switch statement to compare a hashed node type string
 * (computed using `str2int`) and return a shared pointer to an instance of the
 * corresponding node type from the `hesiod` namespace.
 *
 * @param NodeType The name of the node type (e.g., "Noise", "Remap") to create. This
 * should correspond to a class in the `hesiod` namespace.
 *
 * Example usage:
 * @code
 * switch (str2int(node_type.c_str())) {
 *   HSD_NODE_CASE(Noise);
 *   HSD_NODE_CASE(Remap);
 *   // Add more node types as needed
 *   default:
 *     throw std::invalid_argument("Unknown node type in node_factory: " + node_type);
 * }
 * @endcode
 *
 * The macro expands to:
 * @code
 * case str2int("NodeType"):
 *   return std::make_shared<hesiod::NodeType>(config);
 * @endcode
 */
#define HSD_NODE_CASE(NodeType)                                                          \
  case str2int(#NodeType):                                                               \
    return std::make_shared<hesiod::NodeType>(config);

namespace hesiod
{

/**
 * @brief Factory function to create nodes based on the provided node type.
 *
 * This function creates and returns a shared pointer to a node of the specified type.
 * The node is created based on the string name of the node type (e.g., "Noise", "Remap").
 * If the node type is not recognized, the function throws an `std::invalid_argument`
 * exception.
 *
 * @param node_type A string representing the type of node to create. This string is
 * hashed and compared against known node types.
 * @param config A shared pointer to a `ModelConfig` object, which provides configuration
 * parameters for the node being created.
 *
 * @return A `std::shared_ptr<gnode::Node>` pointing to the newly created node.
 *
 * @throws std::invalid_argument if the `node_type` is not recognized.
 *
 * Example usage:
 * @code
 * auto noise_node = node_factory("Noise", config);
 * auto remap_node = node_factory("Remap", config);
 * @endcode
 */
std::shared_ptr<gnode::Node> node_factory(const std::string           &node_type,
                                          std::shared_ptr<ModelConfig> config);

} // namespace hesiod