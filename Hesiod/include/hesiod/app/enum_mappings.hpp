/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once

#include "highmap/authoring.hpp"
#include "highmap/colormaps.hpp"
#include "highmap/erosion.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/morphology.hpp"
#include "highmap/primitives.hpp"
#include "highmap/range.hpp"

#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

static struct EnumMappings
{

  const std::map<std::string, int> blending_method_map = {
      {"add", BlendingMethod::ADD},
      {"exclusion", BlendingMethod::EXCLUSION_BLEND},
      {"gradients", BlendingMethod::GRADIENTS},
      {"maximum", BlendingMethod::MAXIMUM},
      {"maximum_smooth", BlendingMethod::MAXIMUM_SMOOTH},
      {"minimum", BlendingMethod::MINIMUM},
      {"minimum_smooth", BlendingMethod::MINIMUM_SMOOTH},
      {"multiply", BlendingMethod::MULTIPLY},
      {"multiply_add", BlendingMethod::MULTIPLY_ADD},
      {"negate", BlendingMethod::NEGATE},
      {"overlay", BlendingMethod::OVERLAY},
      {"replace", BlendingMethod::REPLACE},
      {"soft", BlendingMethod::SOFT},
      {"substract", BlendingMethod::SUBSTRACT}};

  const std::map<std::string, int> cmap_map = {
      {"bone", hmap::Cmap::BONE},
      {"gray", hmap::Cmap::GRAY},
      {"jet", hmap::Cmap::JET},
      {"magma", hmap::Cmap::MAGMA},
      {"nipy_spectral", hmap::Cmap::NIPY_SPECTRAL},
      {"terrain", hmap::Cmap::TERRAIN},
      {"viridis", hmap::Cmap::VIRIDIS}};

  const std::map<std::string, int> distance_function_map = {
      {"Chebyshev", hmap::DistanceFunction::CHEBYSHEV},
      {"Euclidian", hmap::DistanceFunction::EUCLIDIAN},
      {"Euclidian/Chebyshev", hmap::DistanceFunction::EUCLISHEV},
      {"Manhattan", hmap::DistanceFunction::MANHATTAN},
  };

  const std::map<std::string, int> distance_transform_type_map = {
      {"Exact", hmap::DistanceTransformType::DT_EXACT},
      {"Approx. (fast)", hmap::DistanceTransformType::DT_APPROX},
      {"Manhattan (fast)", hmap::DistanceTransformType::DT_MANHATTAN},
  };

  const std::map<std::string, int> erosion_profile_map = {
      {"Cosine", hmap::ErosionProfile::COSINE},
      {"Saw sharp", hmap::ErosionProfile::SAW_SHARP},
      {"Saw smooth", hmap::ErosionProfile::SAW_SMOOTH},
      {"Sharp valleys", hmap::ErosionProfile::SHARP_VALLEYS},
      {"Square smooth", hmap::ErosionProfile::SQUARE_SMOOTH},
      {"Triangle Grenier", hmap::ErosionProfile::TRIANGLE_GRENIER},
      {"Triangle sharp", hmap::ErosionProfile::TRIANGLE_SHARP},
      {"Triangle smooth", hmap::ErosionProfile::TRIANGLE_SMOOTH}};

  const std::map<std::string, int> heightmap_export_format_map = {
      {"png (8 bit)", ExportFormat::PNG8BIT},
      {"png (16 bit)", ExportFormat::PNG16BIT},
      {"raw (16 bit, Unity)", ExportFormat::RAW16BIT}};

  const std::map<std::string, int> kernel_type_map = {
      {"biweight", hmap::KernelType::BIWEIGHT},
      {"cone", hmap::KernelType::CONE},
      {"cone_smooth", hmap::KernelType::CONE_SMOOTH},
      {"cubic_pulse", hmap::KernelType::CUBIC_PULSE},
      {"disk", hmap::KernelType::DISK},
      {"lorentzian", hmap::KernelType::LORENTZIAN},
      {"smooth_cosine", hmap::KernelType::SMOOTH_COSINE},
      {"square", hmap::KernelType::SQUARE},
      {"tricube", hmap::KernelType::TRICUBE}};

  const std::map<std::string, int> mask_combine_method_map = {
      {"union", MaskCombineMethod::UNION},
      {"intersection", MaskCombineMethod::INTERSECTION},
      {"exclusion", MaskCombineMethod::EXCLUSION}};

  const std::map<std::string, int> noise_type_map = {
      {"Perlin", hmap::NoiseType::PERLIN},
      {"Perlin (billow)", hmap::NoiseType::PERLIN_BILLOW},
      {"Perlin (half)", hmap::NoiseType::PERLIN_HALF},
      {"OpenSimplex2", hmap::NoiseType::SIMPLEX2},
      {"OpenSimplex2S", hmap::NoiseType::SIMPLEX2S},
      {"Value", hmap::NoiseType::VALUE},
      {"Value (cubic)", hmap::NoiseType::VALUE_CUBIC},
      {"Value (delaunay)", hmap::NoiseType::VALUE_DELAUNAY},
      {"Value (linear)", hmap::NoiseType::VALUE_LINEAR},
      {"Worley", hmap::NoiseType::WORLEY},
      {"Worley (doube)", hmap::NoiseType::WORLEY_DOUBLE},
      {"Worley (value)", hmap::NoiseType::WORLEY_VALUE}};

  const std::map<std::string, int> noise_type_map_fbm = {
      {"Perlin", hmap::NoiseType::PERLIN},
      {"Perlin (billow)", hmap::NoiseType::PERLIN_BILLOW},
      {"Perlin (half)", hmap::NoiseType::PERLIN_HALF},
      {"OpenSimplex2", hmap::NoiseType::SIMPLEX2},
      {"OpenSimplex2S", hmap::NoiseType::SIMPLEX2S},
      {"Value", hmap::NoiseType::VALUE},
      {"Value (cubic)", hmap::NoiseType::VALUE_CUBIC},
      {"Value (linear)", hmap::NoiseType::VALUE_LINEAR},
      {"Worley", hmap::NoiseType::WORLEY},
      {"Worley (doube)", hmap::NoiseType::WORLEY_DOUBLE},
      {"Worley (value)", hmap::NoiseType::WORLEY_VALUE}};

  const std::map<std::string, int> stamping_blend_method_map = {
      {"add", hmap::StampingBlendMethod::ADD},
      {"maximum", hmap::StampingBlendMethod::MAXIMUM},
      // {"maximum_smooth", hmap::StampingBlendMethod::MAXIMUM_SMOOTH},
      {"minimum", hmap::StampingBlendMethod::MINIMUM},
      // {"minimum_smooth", hmap::StampingBlendMethod::MINIMUM_SMOOTH},
      {"multiply", hmap::StampingBlendMethod::MULTIPLY},
      {"substract", hmap::StampingBlendMethod::SUBSTRACT}};

  const std::map<std::string, int> voronoi_return_type_map = {
      {"F1: squared distance to the closest point", hmap::VoronoiReturnType::F1_SQUARED},
      {"F2: squared distance to the second closest point",
       hmap::VoronoiReturnType::F2_SQUARED},
      {"F1 * F2: squared", hmap::VoronoiReturnType::F1TF2_SQUARED},
      {"F1 / F2: squared", hmap::VoronoiReturnType::F1DF2_SQUARED},
      {"F2 - F1: squared", hmap::VoronoiReturnType::F2MF1_SQUARED},
      {"Edge distance exp. decay", hmap::VoronoiReturnType::EDGE_DISTANCE_EXP},
      {"Edge distance squared", hmap::VoronoiReturnType::EDGE_DISTANCE_SQUARED},
      {"Cell value", hmap::VoronoiReturnType::CONSTANT},
      {"Cell value * (F2 - F1)", hmap::VoronoiReturnType::CONSTANT_F2MF1_SQUARED}};

  const std::map<std::string, int> clamping_mode_map = {
      {"Keep positive & clamp", hmap::ClampMode::POSITIVE_ONLY},
      {"Keep negative & clamp", hmap::ClampMode::NEGATIVE_ONLY},
      {"Clamp both", hmap::ClampMode::BOTH},
      {"No clamping", hmap::ClampMode::NONE}};
} enum_mappings;

} // namespace hesiod