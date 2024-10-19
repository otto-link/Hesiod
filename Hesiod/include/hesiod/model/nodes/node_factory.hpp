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
DECLARE_NODE(convolve_svd)
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
DECLARE_NODE(fill_talus)
DECLARE_NODE(fold)
DECLARE_NODE(gain)
DECLARE_NODE(gamma_correction)
DECLARE_NODE(gamma_correction_local)
DECLARE_NODE(gaussian_decay)
DECLARE_NODE(gaussian_pulse)
DECLARE_NODE(gradient)
DECLARE_NODE(gradient_angle)
DECLARE_NODE(gradient_norm)
DECLARE_NODE(gradient_talus)
DECLARE_NODE(heightmap_to_kernel)
DECLARE_NODE(heightmap_to_mask)
DECLARE_NODE(heightmap_to_rgba)
DECLARE_NODE(hydraulic_stream_upscale_amplification)
DECLARE_NODE(import_heightmap)
DECLARE_NODE(import_texture)
DECLARE_NODE(inverse)
DECLARE_NODE(kernel_prim)
DECLARE_NODE(kmeans_clustering2)
DECLARE_NODE(kmeans_clustering3)
DECLARE_NODE(laplace)
DECLARE_NODE(lerp)
DECLARE_NODE(make_binary)
DECLARE_NODE(make_periodic)
DECLARE_NODE(make_periodic_stitching)
DECLARE_NODE(median3x3)
DECLARE_NODE(mix_texture)
DECLARE_NODE(mixer)
DECLARE_NODE(morphological_gradient)
DECLARE_NODE(noise)
DECLARE_NODE(noise_fbm)
DECLARE_NODE(noise_iq)
DECLARE_NODE(noise_jordan)
DECLARE_NODE(noise_parberry);
DECLARE_NODE(noise_pingpong);
DECLARE_NODE(noise_ridged);
DECLARE_NODE(noise_swiss);
DECLARE_NODE(normal_displacement)
DECLARE_NODE(opening)
DECLARE_NODE(paraboloid)
DECLARE_NODE(path)
DECLARE_NODE(path_bezier);
DECLARE_NODE(path_bezier_round);
DECLARE_NODE(path_bspline);
DECLARE_NODE(path_decasteljau);
DECLARE_NODE(path_fractalize);
DECLARE_NODE(path_meanderize);
DECLARE_NODE(path_resample);
DECLARE_NODE(path_sdf);
DECLARE_NODE(path_to_cloud)
DECLARE_NODE(path_to_heightmap)
DECLARE_NODE(plateau)
DECLARE_NODE(preview)
DECLARE_NODE(quilting_expand)
DECLARE_NODE(quilting_shuffle)
DECLARE_NODE(radial_displacement_to_xy)
DECLARE_NODE(recast_cliff)
DECLARE_NODE(recast_sag)
DECLARE_NODE(recurve_kura)
DECLARE_NODE(recurve_s)
DECLARE_NODE(relative_elevation)
DECLARE_NODE(remap)
DECLARE_NODE(rescale)
DECLARE_NODE(reverse_midpoint)
DECLARE_NODE(ridgelines)
DECLARE_NODE(rift)
DECLARE_NODE(rugosity)
DECLARE_NODE(saturate)
DECLARE_NODE(scan_mask)
DECLARE_NODE(select_angle)
DECLARE_NODE(select_blob_log)
DECLARE_NODE(select_cavities)
DECLARE_NODE(select_gt)
DECLARE_NODE(select_interval)
DECLARE_NODE(select_inward_outward)
DECLARE_NODE(select_midrange)
DECLARE_NODE(select_multiband3)
DECLARE_NODE(select_pulse)
DECLARE_NODE(select_rivers)
DECLARE_NODE(select_transitions)
DECLARE_NODE(set_alpha)
DECLARE_NODE(shape_index)
DECLARE_NODE(sharpen_cone)
DECLARE_NODE(shift_elevation)
DECLARE_NODE(slope)
DECLARE_NODE(smooth_cpulse)
DECLARE_NODE(smooth_fill)
DECLARE_NODE(smooth_fill_holes)
DECLARE_NODE(smooth_fill_smear_peaks)
DECLARE_NODE(smoothstep3)
DECLARE_NODE(smoothstep5)
DECLARE_NODE(stamping)
DECLARE_NODE(stratify)
DECLARE_NODE(stratify_oblique)
DECLARE_NODE(step)
DECLARE_NODE(thermal)
DECLARE_NODE(thermal_rib)
DECLARE_NODE(translate)
DECLARE_NODE(unsphericity)
DECLARE_NODE(valley_width)
DECLARE_NODE(warp)
DECLARE_NODE(warp_downslope)
DECLARE_NODE(wave_dune)
DECLARE_NODE(wave_sine)
DECLARE_NODE(wave_square)
DECLARE_NODE(wave_triangular)
DECLARE_NODE(white)
DECLARE_NODE(white_density_map)
DECLARE_NODE(white_sparse)
DECLARE_NODE(zeroed_edges)
DECLARE_NODE(zoom)

} // namespace hesiod
