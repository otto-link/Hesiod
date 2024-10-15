/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <stdexcept>

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/node_factory.hpp"

namespace hesiod
{

// HELPER
constexpr unsigned int str2int(const char *str, int h = 0)
{
  // https://stackoverflow.com/questions/16388510
  return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

std::map<std::string, std::string> get_node_inventory()
{
  std::map<std::string, std::string> node_inventory = {
      {"Abs", "Math/Base"},
      {"AbsSmooth", "Math/Base"},
      {"AccumulationCurvature", "Features/Landform"},
      {"Blend", "Operator/Blend"},
      {"Brush", "Primitive/Authoring"},
      {"Bump", "Primitive/Function"},
      {"Caldera", "Primitive/Geological"},
      {"Clamp", "Filter/Range"},
      {"Closing", "Operator/Morphology"},
      {"Cloud", "Geometry/Cloud"},
      {"CloudMerge", "Geometry/Cloud"},
      {"CloudLattice", "Geometry/Cloud"},
      {"CloudRandom", "Geometry/Cloud"},
      {"CloudRemapValues", "Geometry/Cloud"},
      {"CloudSDF", "Geometry/Cloud"},
      {"CloudToArrayInterp", "Geometry/Cloud"},
      {"CloudToPath", "Geometry/Cloud"},
      {"ColorizeGradient", "Texture"},
      {"ColorizeSolid", "Texture"},
      {"CombineMask", "Mask"},
      {"Cos", "Math/Base"},
      {"Crater", "Primitive/Geological"},
      {"Dendry", "Primitive/Coherent"},
      {"DepressionFilling", "Erosion"},
      {"Detrend", "Filter/Recurve"},
      {"DiffusionLimitedAggregation", "Primitive/Coherent"},
      {"Dilation", "Operator/Morphology"},
      {"DistanceTransform", "Operator/Morphology"},
      {"Erosion", "Operator/Morphology"},
      {"ExpandShrink", "Filter/Recast"},
      {"ExportAsset", "IO/Files"},
      {"ExportCloud", "IO/Files"},
      {"ExportHeightmap", "IO/Files"},
      {"ExportNormalMap", "IO/Files"},
      {"ExportPath", "IO/Files"},
      {"ExportTexture", "IO/Files"},
      {"Falloff", "Math/Boundaries"},
      {"FillTalus", "Operator/Transform"},
      {"Fold", "Filter/Recast"},
      {"Gain", "Filter/Recurve"},
      {"GammaCorrection", "Filter/Recurve"},
      {"GammaCorrectionLocal", "Filter/Recurve"},
      {"GaussianDecay", "Math/Base"},
      {"GaussianPulse", "Primitive/Function"},
      {"Gradient", "Math/Gradient"},
      {"GradientAngle", "Math/Gradient"},
      {"GradientNorm", "Math/Gradient"},
      {"GradientTalus", "Math/Gradient"},
      {"HydraulicStreamUpscaleAmplification", "Erosion/Hydraulic"},
      {"ImportTexture", "Texture"},
      {"Laplace", "Filter/Smoothing"},
      {"Lerp", "Math/Base"},
      {"MakeBinary", "Operator/Morphology"},
      {"MakePeriodic", "Operator/Tiling"},
      {"MakePeriodicStitching", "Operator/Tiling"},
      {"Median3x3", "Filter/Smoothing"},
      {"MixTexture", "Texture"},
      {"Mixer", "Operator/Blend"},
      {"Noise", "Primitive/Coherent"},
      {"NoiseFbm", "Primitive/Coherent"},
      {"NoiseIq", "Primitive/Coherent"},
      {"NoiseJordan", "Primitive/Coherent"},
      {"Path", "Geometry/Path"},
      {"PathSDF", "Geometry/Path"},
      {"Preview", "Debug"},
      {"Remap", "Filter/Range"},
      {"Rift", "Primitive/Function"},
      {"SetAlpha", "Texture"},
      {"SmoothFill", "Filter/Smoothing"}};

  return node_inventory;
}

std::shared_ptr<gnode::Node> node_factory(const std::string           &node_type,
                                          std::shared_ptr<ModelConfig> config)
{
  auto sptr = std::make_shared<hesiod::BaseNode>(node_type, config);

  switch (str2int(node_type.c_str()))
  {
    SETUP_NODE(Abs, abs);
    SETUP_NODE(AbsSmooth, abs_smooth);
    SETUP_NODE(AccumulationCurvature, accumulation_curvature);
    SETUP_NODE(Blend, blend);
    SETUP_NODE(Brush, brush);
    SETUP_NODE(Bump, bump);
    SETUP_NODE(Caldera, caldera);
    SETUP_NODE(Clamp, clamp);
    SETUP_NODE(Closing, closing);
    SETUP_NODE(Cloud, cloud);
    SETUP_NODE(CloudMerge, cloud_merge);
    SETUP_NODE(CloudLattice, cloud_lattice);
    SETUP_NODE(CloudRandom, cloud_random);
    SETUP_NODE(CloudRemapValues, cloud_remap_values);
    SETUP_NODE(CloudSDF, cloud_sdf);
    SETUP_NODE(CloudToArrayInterp, cloud_to_array_interp);
    SETUP_NODE(CloudToPath, cloud_to_path);
    SETUP_NODE(ColorizeGradient, colorize_gradient);
    SETUP_NODE(ColorizeSolid, colorize_solid);
    SETUP_NODE(CombineMask, combine_mask);
    SETUP_NODE(Cos, cos);
    SETUP_NODE(Crater, crater);
    SETUP_NODE(Dendry, dendry);
    SETUP_NODE(DepressionFilling, depression_filling);
    SETUP_NODE(Detrend, detrend);
    SETUP_NODE(DiffusionLimitedAggregation, diffusion_limited_aggregation);
    SETUP_NODE(Dilation, dilation);
    SETUP_NODE(DistanceTransform, distance_transform);
    SETUP_NODE(Erosion, erosion);
    SETUP_NODE(ExpandShrink, expand_shrink);
    SETUP_NODE(ExportAsset, export_asset);
    SETUP_NODE(ExportCloud, export_cloud);
    SETUP_NODE(ExportHeightmap, export_heightmap);
    SETUP_NODE(ExportNormalMap, export_normal_map);
    SETUP_NODE(ExportPath, export_path);
    SETUP_NODE(ExportTexture, export_texture);
    SETUP_NODE(Falloff, falloff);
    SETUP_NODE(FillTalus, fill_talus);
    SETUP_NODE(Fold, fold);
    SETUP_NODE(Gain, gain);
    SETUP_NODE(GammaCorrection, gamma_correction);
    SETUP_NODE(GammaCorrectionLocal, gamma_correction_local);
    SETUP_NODE(GaussianDecay, gaussian_decay);
    SETUP_NODE(GaussianPulse, gaussian_pulse);
    SETUP_NODE(Gradient, gradient);
    SETUP_NODE(GradientAngle, gradient_angle);
    SETUP_NODE(GradientNorm, gradient_norm);
    SETUP_NODE(GradientTalus, gradient_talus);
    SETUP_NODE(HydraulicStreamUpscaleAmplification,
               hydraulic_stream_upscale_amplification);
    SETUP_NODE(ImportTexture, import_texture);
    SETUP_NODE(Laplace, laplace);
    SETUP_NODE(Lerp, lerp);
    SETUP_NODE(MixTexture, mix_texture);
    SETUP_NODE(MakeBinary, make_binary);
    SETUP_NODE(MakePeriodic, make_periodic);
    SETUP_NODE(MakePeriodicStitching, make_periodic_stitching);
    SETUP_NODE(Median3x3, median3x3);
    SETUP_NODE(Mixer, mixer);
    SETUP_NODE(Noise, noise);
    SETUP_NODE(NoiseFbm, noise_fbm);
    SETUP_NODE(NoiseIq, noise_iq);
    SETUP_NODE(NoiseJordan, noise_iq);
    SETUP_NODE(Path, path);
    SETUP_NODE(PathSDF, path_sdf);
    SETUP_NODE(Preview, preview);
    SETUP_NODE(Remap, remap);
    SETUP_NODE(Rift, rift);
    SETUP_NODE(SetAlpha, set_alpha);
    SETUP_NODE(SmoothFill, smooth_fill);
  default:
    throw std::invalid_argument("Unknown node type in node_factory: " + node_type);
  }

  return sptr;
}

} // namespace hesiod
