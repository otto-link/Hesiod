/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/control_node.hpp"
#include "hesiod/utils.hpp"

namespace hesiod::cnode
{

std::shared_ptr<gnode::Node> create_control_node_from_type(
    std::string     node_type,
    std::string     node_id,
    hmap::Vec2<int> shape,
    hmap::Vec2<int> tiling,
    float           overlap)
{

  // clang-format off
  switch(str2int(node_type.c_str()))
  {
  case str2int("Abs"):
    return std::make_shared<hesiod::cnode::Abs>(node_id);
  case str2int("AbsSmooth"):
    return std::make_shared<hesiod::cnode::AbsSmooth>(node_id);
  case str2int("BaseElevation"):
    return std::make_shared<hesiod::cnode::BaseElevation>(node_id, shape, tiling, overlap);
  case str2int("BezierPath"):
    return std::make_shared<hesiod::cnode::BezierPath>(node_id);
  case str2int("BiquadPulse"):
    return std::make_shared<hesiod::cnode::BiquadPulse>(node_id, shape, tiling, overlap);
  case str2int("Blend"):
    return std::make_shared<hesiod::cnode::Blend>(node_id);
  case str2int("Brush"):
    return std::make_shared<hesiod::cnode::Brush>(node_id, shape, tiling, overlap);
  case str2int("Bump"):
    return std::make_shared<hesiod::cnode::Bump>(node_id, shape, tiling, overlap);
  case str2int("Caldera"):
    return std::make_shared<hesiod::cnode::Caldera>(node_id, shape, tiling, overlap);
  case str2int("Checkerboard"):
    return std::make_shared<hesiod::cnode::Checkerboard>(node_id, shape, tiling, overlap);
  case str2int("Clamp"):
    return std::make_shared<hesiod::cnode::Clamp>(node_id);
  case str2int("Clone"):
    return std::make_shared<hesiod::cnode::Clone>(node_id);
  case str2int("Cloud"):
    return std::make_shared<hesiod::cnode::Cloud>(node_id);
  case str2int("CloudToArrayInterp"):
    return std::make_shared<hesiod::cnode::CloudToArrayInterp>(node_id, shape, tiling, overlap);
  case str2int("Colorize"):
    return std::make_shared<hesiod::cnode::Colorize>(node_id);
  case str2int("ColorizeSolid"):
    return std::make_shared<hesiod::cnode::ColorizeSolid>(node_id);
  case str2int("CombineMask"):
    return std::make_shared<hesiod::cnode::CombineMask>(node_id);
  case str2int("ConvolveSVD"):
    return std::make_shared<hesiod::cnode::ConvolveSVD>(node_id);
  case str2int("Debug"):
    return std::make_shared<hesiod::cnode::Debug>(node_id);
  case str2int("Dendry"):
    return std::make_shared<hesiod::cnode::Dendry>(node_id, shape, tiling, overlap);
  case str2int("DepressionFilling"):
    return std::make_shared<hesiod::cnode::DepressionFilling>(node_id);
  case str2int("DigPath"):
    return std::make_shared<hesiod::cnode::DigPath>(node_id);
  case str2int("DistanceTransform"):
    return std::make_shared<hesiod::cnode::DistanceTransform>(node_id);
  case str2int("Equalize"):
    return std::make_shared<hesiod::cnode::Equalize>(node_id);
  case str2int("ErosionMaps"):
    return std::make_shared<hesiod::cnode::ErosionMaps>(node_id);
  case str2int("ExpandShrinkDirectional"):
    return std::make_shared<hesiod::cnode::ExpandShrinkDirectional>(node_id);
  case str2int("ExpandShrink"):
    return std::make_shared<hesiod::cnode::ExpandShrink>(node_id);
  case str2int("Export"):
    return std::make_shared<hesiod::cnode::Export>(node_id);
  case str2int("ExportRGB"):
    return std::make_shared<hesiod::cnode::ExportRGB>(node_id);
  case str2int("Faceted"):
    return std::make_shared<hesiod::cnode::Faceted>(node_id);
  case str2int("FractalizePath"):
    return std::make_shared<hesiod::cnode::FractalizePath>(node_id);
  case str2int("GaborNoise"):
    return std::make_shared<hesiod::cnode::GaborNoise>(node_id, shape, tiling, overlap);
  case str2int("Gain"):
    return std::make_shared<hesiod::cnode::Gain>(node_id);
  case str2int("GammaCorrection"):
    return std::make_shared<hesiod::cnode::GammaCorrection>(node_id);
  case str2int("GammaCorrectionLocal"):
    return std::make_shared<hesiod::cnode::GammaCorrectionLocal>(node_id);
  case str2int("GaussianPulse"):
    return std::make_shared<hesiod::cnode::GaussianPulse>(node_id, shape, tiling, overlap);
  case str2int("Geomorphons"):
    return std::make_shared<hesiod::cnode::Geomorphons>(node_id);
  case str2int("Gradient"):
    return std::make_shared<hesiod::cnode::Gradient>(node_id);
  case str2int("GradientAngle"):
    return std::make_shared<hesiod::cnode::GradientAngle>(node_id);
  case str2int("GradientNorm"):
    return std::make_shared<hesiod::cnode::GradientNorm>(node_id);
  case str2int("GradientTalus"):
    return std::make_shared<hesiod::cnode::GradientTalus>(node_id);
  case str2int("HydraulicAlgebric"):
    return std::make_shared<hesiod::cnode::HydraulicAlgebric>(node_id);
  case str2int("HydraulicParticle"):
    return std::make_shared<hesiod::cnode::HydraulicParticle>(node_id);
  case str2int("HydraulicRidge"):
    return std::make_shared<hesiod::cnode::HydraulicRidge>(node_id);
  case str2int("HydraulicStream"):
    return std::make_shared<hesiod::cnode::HydraulicStream>(node_id);
  case str2int("HydraulicStreamLog"):
    return std::make_shared<hesiod::cnode::HydraulicStreamLog>(node_id);
  case str2int("HydraulicVpipes"):
    return std::make_shared<hesiod::cnode::HydraulicVpipes>(node_id);
  case str2int("Import"):
    return std::make_shared<hesiod::cnode::Import>(node_id, shape, tiling, overlap);
  case str2int("Inverse"):
    return std::make_shared<hesiod::cnode::Inverse>(node_id);
  case str2int("Kernel"):
    return std::make_shared<hesiod::cnode::Kernel>(node_id);
  case str2int("KmeansClustering2"):
    return std::make_shared<hesiod::cnode::KmeansClustering2>(node_id);
  case str2int("KmeansClustering3"):
    return std::make_shared<hesiod::cnode::KmeansClustering3>(node_id);
  case str2int("Laplace"):
    return std::make_shared<hesiod::cnode::Laplace>(node_id);
  case str2int("LaplaceEdgePreserving"):
    return std::make_shared<hesiod::cnode::LaplaceEdgePreserving>(node_id);
  case str2int("Lerp"):
    return std::make_shared<hesiod::cnode::Lerp>(node_id);
  case str2int("MakeBinary"):
    return std::make_shared<hesiod::cnode::MakeBinary>(node_id);
  case str2int("MeanderizePath"):
    return std::make_shared<hesiod::cnode::MeanderizePath>(node_id);
  case str2int("MeanLocal"):
    return std::make_shared<hesiod::cnode::MeanLocal>(node_id);
  case str2int("Median3x3"):
    return std::make_shared<hesiod::cnode::Median3x3>(node_id);
  case str2int("MinimumLocal"):
    return std::make_shared<hesiod::cnode::MinimumLocal>(node_id);
  case str2int("MixRGB"):
    return std::make_shared<hesiod::cnode::MixRGB>(node_id);
  case str2int("Noise"):
    return std::make_shared<hesiod::cnode::Noise>(node_id, shape, tiling, overlap);
  case str2int("NoiseFbm"):
    return std::make_shared<hesiod::cnode::NoiseFbm>(node_id, shape, tiling, overlap);
  case str2int("NoiseIq"):
    return std::make_shared<hesiod::cnode::NoiseIq>(node_id, shape, tiling, overlap);
  case str2int("NoiseJordan"):
    return std::make_shared<hesiod::cnode::NoiseJordan>(node_id, shape, tiling, overlap);
  case str2int("NoisePingpong"):
    return std::make_shared<hesiod::cnode::NoisePingpong>(node_id, shape, tiling, overlap);
  case str2int("NoiseRidged"):
    return std::make_shared<hesiod::cnode::NoiseRidged>(node_id, shape, tiling, overlap);
  case str2int("NoiseSwiss"):
    return std::make_shared<hesiod::cnode::NoiseSwiss>(node_id, shape, tiling, overlap);
  case str2int("NormalDisplacement"):
    return std::make_shared<hesiod::cnode::NormalDisplacement>(node_id);
  case str2int("OneMinus"):
    return std::make_shared<hesiod::cnode::OneMinus>(node_id);
  case str2int("Path"):
    return std::make_shared<hesiod::cnode::Path>(node_id);
  case str2int("PathFinding"):
    return std::make_shared<hesiod::cnode::PathFinding>(node_id);
  case str2int("PathToHeightmap"):
    return std::make_shared<hesiod::cnode::PathToHeightmap>(node_id, shape, tiling, overlap);
  case str2int("Peak"):
    return std::make_shared<hesiod::cnode::Peak>(node_id, shape, tiling, overlap);
  case str2int("Plateau"):
    return std::make_shared<hesiod::cnode::Plateau>(node_id);
  case str2int("Preview"):
    return std::make_shared<hesiod::cnode::Preview>(node_id);
  case str2int("PreviewColorize"):
    return std::make_shared<hesiod::cnode::PreviewColorize>(node_id);
  case str2int("RecastCanyon"):
    return std::make_shared<hesiod::cnode::RecastCanyon>(node_id);
  case str2int("RecastCliff"):
    return std::make_shared<hesiod::cnode::RecastCliff>(node_id);
  case str2int("RecastCliffDirectional"):
    return std::make_shared<hesiod::cnode::RecastCliffDirectional>(node_id);
  case str2int("RecastPeak"):
    return std::make_shared<hesiod::cnode::RecastPeak>(node_id);
  case str2int("RecastRockySlopes"):
    return std::make_shared<hesiod::cnode::RecastRockySlopes>(node_id);
  case str2int("Recurve"):
    return std::make_shared<hesiod::cnode::Recurve>(node_id);
  case str2int("RecurveKura"):
    return std::make_shared<hesiod::cnode::RecurveKura>(node_id);
  case str2int("RecurveS"):
    return std::make_shared<hesiod::cnode::RecurveS>(node_id);
  case str2int("RelativeElevation"):
    return std::make_shared<hesiod::cnode::RelativeElevation>(node_id);
  case str2int("Remap"):
    return std::make_shared<hesiod::cnode::Remap>(node_id);
  case str2int("Rescale"):
    return std::make_shared<hesiod::cnode::Rescale>(node_id);
  case str2int("Rugosity"):
    return std::make_shared<hesiod::cnode::Rugosity>(node_id);
  case str2int("SedimentDeposition"):
    return std::make_shared<hesiod::cnode::SedimentDeposition>(node_id);
  case str2int("SelectBlobLog"):
    return std::make_shared<hesiod::cnode::SelectBlobLog>(node_id);
  case str2int("SelectCavities"):
    return std::make_shared<hesiod::cnode::SelectCavities>(node_id);
  case str2int("SelectElevationSlope"):
    return std::make_shared<hesiod::cnode::SelectElevationSlope>(node_id);
  case str2int("SelectGradientNorm"):
    return std::make_shared<hesiod::cnode::SelectGradientNorm>(node_id);
  case str2int("SelectEq"):
    return std::make_shared<hesiod::cnode::SelectEq>(node_id);
  case str2int("SelectInterval"):
    return std::make_shared<hesiod::cnode::SelectInterval>(node_id);
  case str2int("SelectPulse"):
    return std::make_shared<hesiod::cnode::SelectPulse>(node_id);
  case str2int("SelectRivers"):
    return std::make_shared<hesiod::cnode::SelectRivers>(node_id);
  case str2int("SelectTransitions"):
    return std::make_shared<hesiod::cnode::SelectTransitions>(node_id);
  case str2int("SharpenCone"):
    return std::make_shared<hesiod::cnode::SharpenCone>(node_id);
  case str2int("Slope"):
    return std::make_shared<hesiod::cnode::Slope>(node_id, shape, tiling, overlap);
  case str2int("SmoothCpulse"):
    return std::make_shared<hesiod::cnode::SmoothCpulse>(node_id);
  case str2int("SmoothFill"):
    return std::make_shared<hesiod::cnode::SmoothFill>(node_id);
  case str2int("SmoothFillHoles"):
    return std::make_shared<hesiod::cnode::SmoothFillHoles>(node_id);
  case str2int("SmoothFillSmearPeaks"):
    return std::make_shared<hesiod::cnode::SmoothFillSmearPeaks>(node_id);
  case str2int("SteepenConvective"):
    return std::make_shared<hesiod::cnode::SteepenConvective>(node_id);
  case str2int("Step"):
    return std::make_shared<hesiod::cnode::Step>(node_id, shape, tiling, overlap);
  case str2int("StratifyMultiscale"):
    return std::make_shared<hesiod::cnode::StratifyMultiscale>(node_id);
  case str2int("StratifyOblique"):
    return std::make_shared<hesiod::cnode::StratifyOblique>(node_id);
  case str2int("Thermal"):
    return std::make_shared<hesiod::cnode::Thermal>(node_id);
  case str2int("ThermalAutoBedrock"):
    return std::make_shared<hesiod::cnode::ThermalAutoBedrock>(node_id);
  case str2int("ThermalFlatten"):
    return std::make_shared<hesiod::cnode::ThermalFlatten>(node_id);
  case str2int("ThermalScree"):
    return std::make_shared<hesiod::cnode::ThermalScree>(node_id);
  case str2int("ToKernel"):
    return std::make_shared<hesiod::cnode::ToKernel>(node_id);
  case str2int("ToMask"):
    return std::make_shared<hesiod::cnode::ToMask>(node_id);
  case str2int("ValleyWidth"):
    return std::make_shared<hesiod::cnode::ValleyWidth>(node_id);
  case str2int("WaveDune"):
    return std::make_shared<hesiod::cnode::WaveDune>(node_id, shape, tiling, overlap);
  case str2int("WaveSine"):
    return std::make_shared<hesiod::cnode::WaveSine>(node_id, shape, tiling, overlap);
  case str2int("WaveSquare"):
    return std::make_shared<hesiod::cnode::WaveSquare>(node_id, shape, tiling, overlap);
  case str2int("WaveTriangular"):
    return std::make_shared<hesiod::cnode::WaveTriangular>(node_id, shape, tiling, overlap);
  case str2int("Warp"):
    return std::make_shared<hesiod::cnode::Warp>(node_id);
  case str2int("WarpDownslope"):
    return std::make_shared<hesiod::cnode::WarpDownslope>(node_id);
  case str2int("White"):
    return std::make_shared<hesiod::cnode::White>(node_id, shape, tiling, overlap);
  case str2int("WhiteDensityMap"):
    return std::make_shared<hesiod::cnode::WhiteDensityMap>(node_id);
  case str2int("WhiteSparse"):
    return std::make_shared<hesiod::cnode::WhiteSparse>(node_id, shape, tiling, overlap);
  case str2int("Wrinkle"):
    return std::make_shared<hesiod::cnode::Wrinkle>(node_id);
  case str2int("ZeroedEdges"):
    return std::make_shared<hesiod::cnode::ZeroedEdges>(node_id);
  default:
    // clang-format on
    {
      LOG_ERROR("unknown node type: [%s]", node_type.c_str());
      throw std::runtime_error("adding an unknown node type");
      break;
    }
  }
}

} // namespace hesiod::cnode
