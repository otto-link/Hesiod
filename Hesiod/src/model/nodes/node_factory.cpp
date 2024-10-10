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
      {"CloudLattice", "Geometry/Cloud"},
      {"ColorizeGradient", "Texture"},
      {"ExportHeightmap", "IO/Files"},
      {"Falloff", "Math/Boundaries"},
      {"GammaCorrection", "Filter/Recurve"},
      {"Gradient", "Math/Gradient"},
      {"GradientNorm", "Math/Gradient"},
      {"HydraulicStreamUpscaleAmplification", "Erosion/Hydraulic"},
      {"MixTexture", "Texture"},
      {"Noise", "Primitive/Coherent"},
      {"NoiseFbm", "Primitive/Coherent"},
      {"NoiseIq", "Primitive/Coherent"},
      {"NoiseJordan", "Primitive/Coherent"},
      {"Path", "Geometry/Path"},
      {"PathSDF", "Geometry/Path"},
      {"Remap", "Filter/Range"},
      {"Rift", "Primitive/Function"},
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
    SETUP_NODE(CloudLattice, cloud_lattice);
    SETUP_NODE(ColorizeGradient, colorize_gradient);
    SETUP_NODE(ExportHeightmap, export_heightmap);
    SETUP_NODE(Falloff, falloff);
    SETUP_NODE(GammaCorrection, gamma_correction);
    SETUP_NODE(Gradient, gradient);
    SETUP_NODE(GradientNorm, gradient_norm);
    SETUP_NODE(HydraulicStreamUpscaleAmplification,
               hydraulic_stream_upscale_amplification);
    SETUP_NODE(MixTexture, mix_texture);
    SETUP_NODE(Noise, noise);
    SETUP_NODE(NoiseFbm, noise_fbm);
    SETUP_NODE(NoiseIq, noise_iq);
    SETUP_NODE(NoiseJordan, noise_iq);
    SETUP_NODE(Path, path);
    SETUP_NODE(PathSDF, path_sdf);
    SETUP_NODE(Remap, remap);
    SETUP_NODE(Rift, rift);
    SETUP_NODE(SmoothFill, smooth_fill);
  default:
    throw std::invalid_argument("Unknown node type in node_factory: " + node_type);
  }

  return sptr;
}

} // namespace hesiod
