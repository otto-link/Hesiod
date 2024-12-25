/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include "highmap/authoring.hpp"
#include "highmap/colormaps.hpp"
#include "highmap/erosion.hpp"
#include "highmap/kernels.hpp"
#include "highmap/math.hpp"
#include "highmap/primitives.hpp"

namespace hesiod
{

enum BlendingMethod : int
{
  ADD,
  EXCLUSION_BLEND,
  GRADIENTS,
  MAXIMUM,
  MAXIMUM_SMOOTH,
  MINIMUM,
  MINIMUM_SMOOTH,
  MULTIPLY,
  MULTIPLY_ADD,
  NEGATE,
  OVERLAY,
  SOFT,
  SUBSTRACT,
};

enum ExportFormat : int
{
  PNG8BIT,
  PNG16BIT,
  RAW16BIT,
};

enum MaskCombineMethod : int
{
  UNION,
  INTERSECTION,
  EXCLUSION,
};

/**
 * @brief Plain text / enumerate mapping for the blending methods.
 */
static std::map<std::string, int> blending_method_map = {
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
    {"soft", BlendingMethod::SOFT},
    {"substract", BlendingMethod::SUBSTRACT}};

/**
 * @brief Plain text / enumerate mapping for the HighMap colormaps.
 */
static std::map<std::string, int> cmap_map = {
    {"bone", hmap::Cmap::BONE},
    {"gray", hmap::Cmap::GRAY},
    {"jet", hmap::Cmap::JET},
    {"magma", hmap::Cmap::MAGMA},
    {"nipy_spectral", hmap::Cmap::NIPY_SPECTRAL},
    {"terrain", hmap::Cmap::TERRAIN},
    {"viridis", hmap::Cmap::VIRIDIS}};

/**
 * @brief Plain text / enumerate mapping for the HighMap distance functions.
 */
static std::map<std::string, int> distance_function_map = {
    {"Chebyshev", hmap::DistanceFunction::CHEBYSHEV},
    {"Euclidian", hmap::DistanceFunction::EUCLIDIAN},
    {"Euclidian/Chebyshev", hmap::DistanceFunction::EUCLISHEV},
    {"Manhattan", hmap::DistanceFunction::MANHATTAN},
};

/**
 * @brief Plain text / enumerate mapping for the HighMap erosion profiles (procedural
 * erosion).
 */
static std::map<std::string, int> erosion_profile_map = {
    {"Cosine", hmap::ErosionProfile::COSINE},
    {"Saw sharp", hmap::ErosionProfile::SAW_SHARP},
    {"Saw smooth", hmap::ErosionProfile::SAW_SMOOTH},
    {"Sharp valleys", hmap::ErosionProfile::SHARP_VALLEYS},
    {"Square smooth", hmap::ErosionProfile::SQUARE_SMOOTH},
    {"Triangle Grenier", hmap::ErosionProfile::TRIANGLE_GRENIER},
    {"Triangle sharp", hmap::ErosionProfile::TRIANGLE_SHARP},
    {"Triangle smooth", hmap::ErosionProfile::TRIANGLE_SMOOTH}};

/**
 * @brief Plain text / enumerate mapping for the heightmap export formats (see
 * ExportHeightmap node).
 */
static std::map<std::string, int> heightmap_export_format_map = {
    {"png (8 bit)", ExportFormat::PNG8BIT},
    {"png (16 bit)", ExportFormat::PNG16BIT},
    {"raw (16 bit, Unity)", ExportFormat::RAW16BIT}};

/**
 * @brief Plain text / enumerate mapping for the kernels.
 */
static std::map<std::string, int> kernel_type_map = {
    {"biweight", hmap::KernelType::BIWEIGHT},
    {"cone", hmap::KernelType::CONE},
    {"cone_smooth", hmap::KernelType::CONE_SMOOTH},
    {"cubic_pulse", hmap::KernelType::CUBIC_PULSE},
    {"disk", hmap::KernelType::DISK},
    {"lorentzian", hmap::KernelType::LORENTZIAN},
    {"smooth_cosine", hmap::KernelType::SMOOTH_COSINE},
    {"square", hmap::KernelType::SQUARE},
    {"tricube", hmap::KernelType::TRICUBE}};

/**
 * @brief Plain text / enumerate mapping for the mask combine method.
 */
static std::map<std::string, int> mask_combine_method_map = {
    {"union", MaskCombineMethod::UNION},
    {"intersection", MaskCombineMethod::INTERSECTION},
    {"exclusion", MaskCombineMethod::EXCLUSION}};

/**
 * @brief Plain text / enumerate mapping for the HighMap noise types.
 */
static std::map<std::string, int> noise_type_map = {
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

/**
 * @brief Plain text / enumerate mapping for the HighMap noise types, with some value
 * noises removed (delaunay, linear and thinplate) that can be very slow at high
 * wavenumbers
 */
static std::map<std::string, int> noise_type_map_fbm = {
    {"Perlin", hmap::NoiseType::PERLIN},
    {"Perlin (billow)", hmap::NoiseType::PERLIN_BILLOW},
    {"Perlin (half)", hmap::NoiseType::PERLIN_HALF},
    {"OpenSimplex2", hmap::NoiseType::SIMPLEX2},
    {"OpenSimplex2S", hmap::NoiseType::SIMPLEX2S},
    {"Value", hmap::NoiseType::VALUE},
    {"Value (cubic)", hmap::NoiseType::VALUE_CUBIC},
    {"Worley", hmap::NoiseType::WORLEY},
    {"Worley (doube)", hmap::NoiseType::WORLEY_DOUBLE},
    {"Worley (value)", hmap::NoiseType::WORLEY_VALUE}};

/**
 * @brief Plain text / enumerate mapping for the stamping blend method.
 */
static std::map<std::string, int> stamping_blend_method_map = {
    {"add", hmap::StampingBlendMethod::ADD},
    {"maximum", hmap::StampingBlendMethod::MAXIMUM},
    // {"maximum_smooth", hmap::StampingBlendMethod::MAXIMUM_SMOOTH},
    {"minimum", hmap::StampingBlendMethod::MINIMUM},
    // {"minimum_smooth", hmap::StampingBlendMethod::MINIMUM_SMOOTH},
    {"multiply", hmap::StampingBlendMethod::MULTIPLY},
    {"substract", hmap::StampingBlendMethod::SUBSTRACT}};

} // namespace hesiod