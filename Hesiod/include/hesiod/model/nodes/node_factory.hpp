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
#include "hesiod/model/nodes/base_node.hpp"

#define SETUP_NODE(NodeType, node_type)                                                  \
  case str2int(#NodeType):                                                               \
    setup_##node_type##_node(sptr.get());                                                \
    sptr->set_compute_fct(&compute_##node_type##_node);                                  \
    break;

#define DECLARE_NODE(node_type)                                                          \
  void setup_##node_type##_node(BaseNode *p_node);                                       \
  void compute_##node_type##_node(BaseNode *p_node);

namespace hesiod
{

/**
 * @brief Retrieves a map of node inventory.
 *
 * This function returns a map where the key represents the node type and the value
 * represents the corresponding node category.
 *
 * @return std::map<std::string, std::string>
 * A map containing node identifiers as keys and node descriptions as values.
 */
std::map<std::string, std::string> get_node_inventory();

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

// nodes functions

DECLARE_NODE(abs)
DECLARE_NODE(abs_smooth)
DECLARE_NODE(accumulation_curvature)
DECLARE_NODE(blend)
DECLARE_NODE(brush)
DECLARE_NODE(bump)
DECLARE_NODE(caldera)
DECLARE_NODE(clamp)
DECLARE_NODE(closing)
DECLARE_NODE(cloud)
DECLARE_NODE(cloud_lattice)
DECLARE_NODE(cloud_merge)
DECLARE_NODE(cloud_random)
DECLARE_NODE(cloud_remap_values)
DECLARE_NODE(cloud_sdf)
DECLARE_NODE(cloud_to_array_interp)
DECLARE_NODE(cloud_to_path)
DECLARE_NODE(colorize_gradient)
DECLARE_NODE(colorize_solid)
DECLARE_NODE(combine_mask)
DECLARE_NODE(cos)
DECLARE_NODE(crater)
DECLARE_NODE(dendry)
DECLARE_NODE(depression_filling)
DECLARE_NODE(detrend)
DECLARE_NODE(diffusion_limited_aggregation)
DECLARE_NODE(dilation)
DECLARE_NODE(distance_transform)
DECLARE_NODE(erosion)
DECLARE_NODE(expand_shrink)
DECLARE_NODE(export_asset)
DECLARE_NODE(export_cloud)
DECLARE_NODE(export_heightmap)
DECLARE_NODE(export_normal_map)
DECLARE_NODE(export_path)
DECLARE_NODE(export_texture)
DECLARE_NODE(falloff)
DECLARE_NODE(fold)
DECLARE_NODE(gain)
DECLARE_NODE(gamma_correction)
DECLARE_NODE(gamma_correction_local)
DECLARE_NODE(gradient)
DECLARE_NODE(gradient_norm)
DECLARE_NODE(hydraulic_stream_upscale_amplification)
DECLARE_NODE(mix_texture)
DECLARE_NODE(noise)
DECLARE_NODE(noise_fbm)
DECLARE_NODE(noise_iq)
DECLARE_NODE(noise_jordan)
DECLARE_NODE(path)
DECLARE_NODE(path_sdf)
DECLARE_NODE(preview)
DECLARE_NODE(remap)
DECLARE_NODE(rift)
DECLARE_NODE(smooth_fill)

} // namespace hesiod