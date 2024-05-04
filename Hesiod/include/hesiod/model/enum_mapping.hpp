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
inline std::map<std::string, int> blending_method_map = {
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
inline std::map<std::string, int> cmap_map = {
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
inline std::map<std::string, int> heightmap_export_format_map = {
    {"png (8 bit)", ExportFormat::PNG8BIT},
    {"png (16 bit)", ExportFormat::PNG16BIT},
    {"raw (16 bit, Unity)", ExportFormat::RAW16BIT}};

/**
 * @brief Plain text / enumerate mapping for the kernels.
 */
inline std::map<std::string, int> kernel_map = {{"cone", Kernel::CONE},
                                                {"cubic_pulse", Kernel::CUBIC_PULSE},
                                                {"lorentzian", Kernel::LORENTZIAN},
                                                {"smooth_cosine", Kernel::SMOOTH_COSINE}};

/**
 * @brief Plain text / enumerate mapping for the mask combine method.
 */
inline std::map<std::string, int> mask_combine_method_map = {
    {"union", MaskCombineMethod::UNION},
    {"intersection", MaskCombineMethod::INTERSECTION},
    {"exclusion", MaskCombineMethod::EXCLUSION}};

/**
 * @brief Plain text / enumerate mapping for the HighMap noise types.
 */
inline std::map<std::string, int> noise_type_map = {
    {"Perlin", hmap::NoiseType::n_perlin},
    {"Perlin (billow)", hmap::NoiseType::n_perlin_billow},
    {"Perlin (half)", hmap::NoiseType::n_perlin_half},
    {"OpenSimplex2", hmap::NoiseType::n_simplex2},
    {"OpenSimplex2S", hmap::NoiseType::n_simplex2s},
    {"Value", hmap::NoiseType::n_value},
    {"Value (cubic)", hmap::NoiseType::n_value_cubic},
    {"Value (delaunay)", hmap::NoiseType::n_value_delaunay},
    {"Value (linear)", hmap::NoiseType::n_value_linear},
    {"Value (thinplate)", hmap::NoiseType::n_value_thinplate},
    {"Worley", hmap::NoiseType::n_worley},
    {"Worley (doube)", hmap::NoiseType::n_worley_double},
    {"Worley (value)", hmap::NoiseType::n_worley_value}};

/**
 * @brief Plain text / enumerate mapping for the HighMap noise types, with some value
 * noises removed (delaunay, linear and thinplate) that can be very slow at high
 * wavenumbers
 */
inline std::map<std::string, int> noise_type_map_fbm = {
    {"Perlin", hmap::NoiseType::n_perlin},
    {"Perlin (billow)", hmap::NoiseType::n_perlin_billow},
    {"Perlin (half)", hmap::NoiseType::n_perlin_half},
    {"OpenSimplex2", hmap::NoiseType::n_simplex2},
    {"OpenSimplex2S", hmap::NoiseType::n_simplex2s},
    {"Value", hmap::NoiseType::n_value},
    {"Value (cubic)", hmap::NoiseType::n_value_cubic},
    {"Worley", hmap::NoiseType::n_worley},
    {"Worley (doube)", hmap::NoiseType::n_worley_double},
    {"Worley (value)", hmap::NoiseType::n_worley_value}};

} // namespace hesiod