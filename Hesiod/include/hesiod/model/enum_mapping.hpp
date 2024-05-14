/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include "highmap/colormaps.hpp"
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

enum Kernel : int
{
  CUBIC_PULSE,
  CONE,
  LORENTZIAN,
  SMOOTH_COSINE
};

enum MaskCombineMethod : int
{
  UNION,
  INTERSECTION,
  EXCLUSION,
};

enum PreviewType : int
{
  GRAYSCALE,
  MAGMA,
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
    {"bone", hmap::cmap::bone},
    {"gray", hmap::cmap::gray},
    {"jet", hmap::cmap::jet},
    {"magma", hmap::cmap::magma},
    {"nipy_spectral", hmap::cmap::nipy_spectral},
    {"terrain", hmap::cmap::terrain},
    {"viridis", hmap::cmap::viridis}};

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
static std::map<std::string, int> kernel_map = {{"cone", Kernel::CONE},
                                                {"cubic_pulse", Kernel::CUBIC_PULSE},
                                                {"lorentzian", Kernel::LORENTZIAN},
                                                {"smooth_cosine", Kernel::SMOOTH_COSINE}};

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
    {"Value (thinplate)", hmap::NoiseType::VALUE_THINPLATE},
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

} // namespace hesiod