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

void setup_abs_node(BaseNode *p_node);
void compute_abs_node(BaseNode *p_node);

void setup_abs_smooth_node(BaseNode *p_node);
void compute_abs_smooth_node(BaseNode *p_node);

void setup_accumulation_curvature_node(BaseNode *p_node);
void compute_accumulation_curvature_node(BaseNode *p_node);

void setup_blend_node(BaseNode *p_node);
void compute_blend_node(BaseNode *p_node);

void setup_brush_node(BaseNode *p_node);
void compute_brush_node(BaseNode *p_node);

void setup_bump_node(BaseNode *p_node);
void compute_bump_node(BaseNode *p_node);

void setup_caldera_node(BaseNode *p_node);
void compute_caldera_node(BaseNode *p_node);

void setup_clamp_node(BaseNode *p_node);
void compute_clamp_node(BaseNode *p_node);

void setup_closing_node(BaseNode *p_node);
void compute_closing_node(BaseNode *p_node);

void setup_cloud_node(BaseNode *p_node);
void compute_cloud_node(BaseNode *p_node);

void setup_cloud_lattice_node(BaseNode *p_node);
void compute_cloud_lattice_node(BaseNode *p_node);

void setup_colorize_gradient_node(BaseNode *p_node);
void compute_colorize_gradient_node(BaseNode *p_node);

void setup_hydraulic_stream_upscale_amplification_node(BaseNode *p_node);
void compute_hydraulic_stream_upscale_amplification_node(BaseNode *p_node);

void setup_export_heightmap_node(BaseNode *p_node);
void compute_export_heightmap_node(BaseNode *p_node);

void setup_falloff_node(BaseNode *p_node);
void compute_falloff_node(BaseNode *p_node);

void setup_gamma_correction_node(BaseNode *p_node);
void compute_gamma_correction_node(BaseNode *p_node);

void setup_gradient_node(BaseNode *p_node);
void compute_gradient_node(BaseNode *p_node);

void setup_gradient_norm_node(BaseNode *p_node);
void compute_gradient_norm_node(BaseNode *p_node);

void setup_mix_texture_node(BaseNode *p_node);
void compute_mix_texture_node(BaseNode *p_node);

void setup_noise_node(BaseNode *p_node);
void compute_noise_node(BaseNode *p_node);

void setup_noise_fbm_node(BaseNode *p_node);
void compute_noise_fbm_node(BaseNode *p_node);

void setup_noise_iq_node(BaseNode *p_node);
void compute_noise_iq_node(BaseNode *p_node);

void setup_noise_jordan_node(BaseNode *p_node);
void compute_noise_jordan_node(BaseNode *p_node);

void setup_path_node(BaseNode *p_node);
void compute_path_node(BaseNode *p_node);

void setup_path_sdf_node(BaseNode *p_node);
void compute_path_sdf_node(BaseNode *p_node);

void setup_remap_node(BaseNode *p_node);
void compute_remap_node(BaseNode *p_node);

void setup_rift_node(BaseNode *p_node);
void compute_rift_node(BaseNode *p_node);

void setup_smooth_fill_node(BaseNode *p_node);
void compute_smooth_fill_node(BaseNode *p_node);

} // namespace hesiod