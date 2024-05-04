/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/model_registry.hpp"
#include "hesiod/model/nodes.hpp"

#include "hesiod/model/nodes/abs.hpp"
#include "hesiod/model/nodes/abs_smooth.hpp"
#include "hesiod/model/nodes/blend.hpp"
#include "hesiod/model/nodes/bump.hpp"
#include "hesiod/model/nodes/clamp.hpp"
#include "hesiod/model/nodes/closing.hpp"
#include "hesiod/model/nodes/cloud.hpp"
#include "hesiod/model/nodes/cloud_to_array_interp.hpp"
#include "hesiod/model/nodes/colorize_cmap.hpp"
#include "hesiod/model/nodes/colorize_gradient.hpp"
#include "hesiod/model/nodes/colorize_solid.hpp"
#include "hesiod/model/nodes/combine_mask.hpp"
#include "hesiod/model/nodes/comment.hpp"
#include "hesiod/model/nodes/cos.hpp"
#include "hesiod/model/nodes/data_preview.hpp"
#include "hesiod/model/nodes/dendry.hpp"
#include "hesiod/model/nodes/depression_filling.hpp"
#include "hesiod/model/nodes/dilation.hpp"
#include "hesiod/model/nodes/erosion.hpp"
#include "hesiod/model/nodes/expand_shrink.hpp"
#include "hesiod/model/nodes/export_asset.hpp"
#include "hesiod/model/nodes/export_heightmap.hpp"
#include "hesiod/model/nodes/export_texture.hpp"
#include "hesiod/model/nodes/fold.hpp"
#include "hesiod/model/nodes/gain.hpp"
#include "hesiod/model/nodes/gamma_correction.hpp"
#include "hesiod/model/nodes/gamma_correction_local.hpp"
#include "hesiod/model/nodes/gaussian_pulse.hpp"
#include "hesiod/model/nodes/gradient_angle.hpp"
#include "hesiod/model/nodes/gradient.hpp"
#include "hesiod/model/nodes/gradient_norm.hpp"
#include "hesiod/model/nodes/gradient_talus.hpp"
#include "hesiod/model/nodes/heightmap_to_mask.hpp"
#include "hesiod/model/nodes/heightmap_to_rgba.hpp"
#include "hesiod/model/nodes/hydraulic_stream.hpp"
#include "hesiod/model/nodes/inverse.hpp"
#include "hesiod/model/nodes/laplace.hpp"
#include "hesiod/model/nodes/lerp.hpp"
#include "hesiod/model/nodes/make_binary.hpp"
#include "hesiod/model/nodes/make_periodic.hpp"
#include "hesiod/model/nodes/make_periodic_stitching.hpp"
#include "hesiod/model/nodes/median_3x3.hpp"
#include "hesiod/model/nodes/mix_texture.hpp"
#include "hesiod/model/nodes/morphological_gradient.hpp"
#include "hesiod/model/nodes/noise_fbm.hpp"
#include "hesiod/model/nodes/noise.hpp"
#include "hesiod/model/nodes/noise_iq.hpp"
#include "hesiod/model/nodes/noise_jordan.hpp"
#include "hesiod/model/nodes/noise_pingpong.hpp"
#include "hesiod/model/nodes/noise_ridged.hpp"
#include "hesiod/model/nodes/noise_swiss.hpp"
#include "hesiod/model/nodes/normal_displacement.hpp"
#include "hesiod/model/nodes/opening.hpp"
#include "hesiod/model/nodes/plateau.hpp"
#include "hesiod/model/nodes/quilting_expand.hpp"
#include "hesiod/model/nodes/quilting_shuffle.hpp"
#include "hesiod/model/nodes/recast_cliff.hpp"
#include "hesiod/model/nodes/recurve_kura.hpp"
#include "hesiod/model/nodes/recurve_s.hpp"
#include "hesiod/model/nodes/remap.hpp"
#include "hesiod/model/nodes/rescale.hpp"
#include "hesiod/model/nodes/saturate.hpp"
#include "hesiod/model/nodes/scan_mask.hpp"
#include "hesiod/model/nodes/select_cavities.hpp"
#include "hesiod/model/nodes/select_gt.hpp"
#include "hesiod/model/nodes/select_interval.hpp"
#include "hesiod/model/nodes/select_pulse.hpp"
#include "hesiod/model/nodes/set_alpha.hpp"
#include "hesiod/model/nodes/sharpen_cone.hpp"
#include "hesiod/model/nodes/slope.hpp"
#include "hesiod/model/nodes/smooth_cpulse.hpp"
#include "hesiod/model/nodes/smooth_fill_holes.hpp"
#include "hesiod/model/nodes/smooth_fill.hpp"
#include "hesiod/model/nodes/smooth_fill_smear_peaks.hpp"
#include "hesiod/model/nodes/smoothstep3.hpp"
#include "hesiod/model/nodes/smoothstep5.hpp"
#include "hesiod/model/nodes/step.hpp"
#include "hesiod/model/nodes/stratify_oblique.hpp"
#include "hesiod/model/nodes/valley_width.hpp"
#include "hesiod/model/nodes/wave_dune.hpp"
#include "hesiod/model/nodes/wave_sine.hpp"
#include "hesiod/model/nodes/wave_square.hpp"
#include "hesiod/model/nodes/wave_triangular.hpp"
#include "hesiod/model/nodes/white.hpp"
#include "hesiod/model/nodes/zeroed_edges.hpp"

namespace hesiod
{

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> register_data_models(
    hesiod::ModelConfig *p_config)
{
  LOG_DEBUG("initializing node registry...");

  auto ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();

  HSD_REGISTER_NODE(hesiod::Abs, "Math/Base");
  HSD_REGISTER_NODE(hesiod::AbsSmooth, "Math/Base");
  HSD_REGISTER_NODE(hesiod::Blend, "Operator/Blend");
  HSD_REGISTER_NODE(hesiod::Bump, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::Clamp, "Filter/Range");
  HSD_REGISTER_NODE(hesiod::Closing, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::Cloud, "Geometry/Cloud");
  HSD_REGISTER_NODE(hesiod::CloudToArrayInterp, "Primitive/Authoring");
  HSD_REGISTER_NODE(hesiod::ColorizeCmap, "Texture");
  HSD_REGISTER_NODE(hesiod::ColorizeGradient, "Texture");
  HSD_REGISTER_NODE(hesiod::ColorizeSolid, "Texture");
  HSD_REGISTER_NODE(hesiod::CombineMask, "Mask");
  HSD_REGISTER_NODE(hesiod::Comment, "Comment");
  HSD_REGISTER_NODE(hesiod::Cos, "Math/Base");
  HSD_REGISTER_NODE(hesiod::DataPreview, "Debug");
  HSD_REGISTER_NODE(hesiod::Dendry, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::DepressionFilling, "Erosion");
  HSD_REGISTER_NODE(hesiod::Dilation, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::Erosion, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::ExpandShrink, "Filter/Recast");
  HSD_REGISTER_NODE(hesiod::ExportAsset, "IO/Files");
  HSD_REGISTER_NODE(hesiod::ExportHeightmap, "IO/Files");
  HSD_REGISTER_NODE(hesiod::ExportTexture, "IO/Files");
  HSD_REGISTER_NODE(hesiod::Fold, "Filter/Recast");
  HSD_REGISTER_NODE(hesiod::Gain, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::GammaCorrection, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::GammaCorrectionLocal, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::GaussianPulse, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::Gradient, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::GradientAngle, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::GradientNorm, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::GradientTalus, "Math/Gradient");
  HSD_REGISTER_NODE(hesiod::HeightMapToMask, "Converter");
  HSD_REGISTER_NODE(hesiod::HeightMapToRGBA, "Converter");
  HSD_REGISTER_NODE(hesiod::HydraulicStream, "Erosion/Hydraulic");
  HSD_REGISTER_NODE(hesiod::Inverse, "Math/Base");
  HSD_REGISTER_NODE(hesiod::Laplace, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::Lerp, "Operator/Blend");
  HSD_REGISTER_NODE(hesiod::MakeBinary, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::MakePeriodic, "Operator/Tiling");
  HSD_REGISTER_NODE(hesiod::MakePeriodicStitching, "Operator/Tiling");
  HSD_REGISTER_NODE(hesiod::Median3x3, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::MixTexture, "Texture");
  HSD_REGISTER_NODE(hesiod::MorphologicalGradient, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::Noise, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseFbm, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseIq, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseJordan, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoisePingpong, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseRidged, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NoiseSwiss, "Primitive/Coherent Noise");
  HSD_REGISTER_NODE(hesiod::NormalDisplacement, "Filter/Recast");
  HSD_REGISTER_NODE(hesiod::Opening, "Operator/Morphology");
  HSD_REGISTER_NODE(hesiod::Plateau, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::QuiltingExpand, "Operator/Expand");
  HSD_REGISTER_NODE(hesiod::QuiltingShuffle, "Operator/Resynthesis");
  HSD_REGISTER_NODE(hesiod::Remap, "Filter/Range");
  HSD_REGISTER_NODE(hesiod::RecastCliff, "Filter/Recast");
  HSD_REGISTER_NODE(hesiod::RecurveKura, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::RecurveS, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::Rescale, "Filter/Range");
  HSD_REGISTER_NODE(hesiod::Saturate, "Filter/Recurve");
  HSD_REGISTER_NODE(hesiod::ScanMask, "Mask/Adjust");
  HSD_REGISTER_NODE(hesiod::SelectCavities, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectGt, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectInterval, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SelectPulse, "Mask/Selector");
  HSD_REGISTER_NODE(hesiod::SetAlpha, "Texture");
  HSD_REGISTER_NODE(hesiod::SharpenCone, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::SmoothCpulse, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::SmoothFill, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::SmoothFillHoles, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::SmoothFillSmearPeaks, "Filter/Smoothing");
  HSD_REGISTER_NODE(hesiod::Smoothstep3, "Math/Base");
  HSD_REGISTER_NODE(hesiod::Smoothstep5, "Math/Base");
  HSD_REGISTER_NODE(hesiod::Slope, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::Step, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::StratifyOblique, "Erosion/Stratify");
  HSD_REGISTER_NODE(hesiod::ValleyWidth, "Features");
  HSD_REGISTER_NODE(hesiod::WaveDune, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::WaveSine, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::WaveSquare, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::WaveTriangular, "Primitive/Function");
  HSD_REGISTER_NODE(hesiod::White, "Primitive/Random");
  HSD_REGISTER_NODE(hesiod::ZeroedEdges, "Math/Boundaries");

  return ret;
}

} // namespace hesiod
