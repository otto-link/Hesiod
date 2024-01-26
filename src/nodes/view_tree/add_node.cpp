/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
#include "macrologger.h"

#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

// HELPER
constexpr unsigned int str2int(const char *str, int h = 0)
{
  // https://stackoverflow.com/questions/16388510
  return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

std::string ViewTree::add_view_node(std::string control_node_type,
                                    std::string node_id)
{
  std::string id;
  if (node_id == "")
    id = control_node_type + "##" + this->get_new_id();
  else
    id = node_id;

  LOG_DEBUG("adding node type: %s", control_node_type.c_str());

  // clang-format off
  switch(str2int(control_node_type.c_str()))
  {
  case str2int("Abs"):
    this->add_node(std::make_shared<hesiod::vnode::ViewAbs>(id));
    break;
  case str2int("AbsSmooth"):
    this->add_node(std::make_shared<hesiod::vnode::ViewAbsSmooth>(id));
    break;
  case str2int("AlterElevation"):
    this->add_node(std::make_shared<hesiod::vnode::ViewAlterElevation>(id));
    break;
  case str2int("BaseElevation"):
    this->add_node(std::make_shared<hesiod::vnode::ViewBaseElevation>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("BezierPath"):
    this->add_node(std::make_shared<hesiod::vnode::ViewBezierPath>(id));
    break;
  case str2int("BiquadPulse"):
    this->add_node(std::make_shared<hesiod::vnode::ViewBiquadPulse>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Blend"):
    this->add_node(std::make_shared<hesiod::vnode::ViewBlend>(id));
    break;
  case str2int("Brush"):
    this->add_node(std::make_shared<hesiod::vnode::ViewBrush>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Bump"):
    this->add_node(std::make_shared<hesiod::vnode::ViewBump>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Caldera"):
    this->add_node(std::make_shared<hesiod::vnode::ViewCaldera>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Checkerboard"):
    this->add_node(std::make_shared<hesiod::vnode::ViewCheckerboard>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Clamp"):
    this->add_node(std::make_shared<hesiod::vnode::ViewClamp>(id));
    break;
  case str2int("Clone"):
    this->add_node(std::make_shared<hesiod::vnode::ViewClone>(id));
    break;
  case str2int("Cloud"):
    this->add_node(std::make_shared<hesiod::vnode::ViewCloud>(id));
    break;
  case str2int("CloudToArrayInterp"):
    this->add_node(std::make_shared<hesiod::vnode::ViewCloudToArrayInterp>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Colorize"):
    this->add_node(std::make_shared<hesiod::vnode::ViewColorize>(id));
    break;
  case str2int("ColorizeSolid"):
    this->add_node(std::make_shared<hesiod::vnode::ViewColorizeSolid>(id));
    break;
  case str2int("CombineMask"):
    this->add_node(std::make_shared<hesiod::vnode::ViewCombineMask>(id));
    break;
  case str2int("ConvolveSVD"):
    this->add_node(std::make_shared<hesiod::vnode::ViewConvolveSVD>(id));
    break;
  case str2int("Debug"):
    this->add_node(std::make_shared<hesiod::vnode::ViewDebug>(id));
    break;
  case str2int("Dendry"):
    this->add_node(std::make_shared<hesiod::vnode::ViewDendry>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("DepressionFilling"):
    this->add_node(std::make_shared<hesiod::vnode::ViewDepressionFilling>(id));
    break;
  case str2int("DigPath"):
    this->add_node(std::make_shared<hesiod::vnode::ViewDigPath>(id));
    break;
  case str2int("DistanceTransform"):
    this->add_node(std::make_shared<hesiod::vnode::ViewDistanceTransform>(id));
    break;
  case str2int("Equalize"):
    this->add_node(std::make_shared<hesiod::vnode::ViewEqualize>(id));
    break;
  case str2int("ErosionMaps"):
    this->add_node(std::make_shared<hesiod::vnode::ViewErosionMaps>(id));
    break;
  case str2int("ExpandShrinkDirectional"):
    this->add_node(std::make_shared<hesiod::vnode::ViewExpandShrinkDirectional>(id));
    break;
  case str2int("ExpandShrink"):
    this->add_node(std::make_shared<hesiod::vnode::ViewExpandShrink>(id));
    break;
  case str2int("Export"):
    this->add_node(std::make_shared<hesiod::vnode::ViewExport>(id));
    break;
  case str2int("ExportRGB"):
    this->add_node(std::make_shared<hesiod::vnode::ViewExportRGB>(id));
    break;
  case str2int("Faceted"):
    this->add_node(std::make_shared<hesiod::vnode::ViewFaceted>(id));
    break;
  case str2int("FbmIqPerlin"):
    this->add_node(std::make_shared<hesiod::vnode::ViewFbmIqPerlin>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("FbmPerlin"):
    this->add_node(std::make_shared<hesiod::vnode::ViewFbmPerlin>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("FbmSimplex"):
    this->add_node(std::make_shared<hesiod::vnode::ViewFbmSimplex>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("FbmWorley"):
    this->add_node(std::make_shared<hesiod::vnode::ViewFbmWorley>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("FractalizePath"):
    this->add_node(std::make_shared<hesiod::vnode::ViewFractalizePath>(id));
    break;
  case str2int("GaborNoise"):
    this->add_node(std::make_shared<hesiod::vnode::ViewGaborNoise>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Gain"):
    this->add_node(std::make_shared<hesiod::vnode::ViewGain>(id));
    break;
  case str2int("GammaCorrection"):
    this->add_node(std::make_shared<hesiod::vnode::ViewGammaCorrection>(id));
    break;
  case str2int("GammaCorrectionLocal"):
    this->add_node(std::make_shared<hesiod::vnode::ViewGammaCorrectionLocal>(id));
    break;
  case str2int("GaussianPulse"):
    this->add_node(std::make_shared<hesiod::vnode::ViewGaussianPulse>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Gradient"):
    this->add_node(std::make_shared<hesiod::vnode::ViewGradient>(id));
    break;
  case str2int("GradientAngle"):
    this->add_node(std::make_shared<hesiod::vnode::ViewGradientAngle>(id));
    break;
  case str2int("GradientNorm"):
    this->add_node(std::make_shared<hesiod::vnode::ViewGradientNorm>(id));
    break;
  case str2int("GradientTalus"):
    this->add_node(std::make_shared<hesiod::vnode::ViewGradientTalus>(id));
    break;
  case str2int("HydraulicAlgebric"):
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicAlgebric>(id));
    break;
  case str2int("HydraulicParticle"):
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicParticle>(id));
    break;
  case str2int("HydraulicRidge"):
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicRidge>(id));
    break;
  case str2int("HydraulicStream"):
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicStream>(id));
    break;
  case str2int("HydraulicStreamLog"):
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicStreamLog>(id));
    break;
  case str2int("HydraulicVpipes"):
    this->add_node(std::make_shared<hesiod::vnode::ViewHydraulicVpipes>(id));
    break;
  case str2int("Import"):
    this->add_node(std::make_shared<hesiod::vnode::ViewImport>(id, shape, tiling, overlap));
    break;
  case str2int("Inverse"):
    this->add_node(std::make_shared<hesiod::vnode::ViewInverse>(id));
    break;
  case str2int("Kernel"):
    this->add_node(std::make_shared<hesiod::vnode::ViewKernel>(id));
    break;
  case str2int("KmeansClustering2"):
    this->add_node(std::make_shared<hesiod::vnode::ViewKmeansClustering2>(id));
    break;
  case str2int("KmeansClustering3"):
    this->add_node(std::make_shared<hesiod::vnode::ViewKmeansClustering3>(id));
    break;
  case str2int("Laplace"):
    this->add_node(std::make_shared<hesiod::vnode::ViewLaplace>(id));
    break;
  case str2int("LaplaceEdgePreserving"):
    this->add_node(std::make_shared<hesiod::vnode::ViewLaplaceEdgePreserving>(id));
    break;
  case str2int("Lerp"):
    this->add_node(std::make_shared<hesiod::vnode::ViewLerp>(id));
    break;
  case str2int("MakeBinary"):
    this->add_node(std::make_shared<hesiod::vnode::ViewMakeBinary>(id));
    break;
  case str2int("MeanderizePath"):
    this->add_node(std::make_shared<hesiod::vnode::ViewMeanderizePath>(id));
    break;
  case str2int("MeanLocal"):
    this->add_node(std::make_shared<hesiod::vnode::ViewMeanLocal>(id));
    break;
  case str2int("Median3x3"):
    this->add_node(std::make_shared<hesiod::vnode::ViewMedian3x3>(id));
    break;
  case str2int("MinimumLocal"):
    this->add_node(std::make_shared<hesiod::vnode::ViewMinimumLocal>(id));
    break;
  case str2int("MixRGB"):
    this->add_node(std::make_shared<hesiod::vnode::ViewMixRGB>(id));
    break;
  case str2int("NormalDisplacement"):
    this->add_node(std::make_shared<hesiod::vnode::ViewNormalDisplacement>(id));
    break;
  case str2int("OneMinus"):
    this->add_node(std::make_shared<hesiod::vnode::ViewOneMinus>(id));
    break;
  case str2int("Path"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPath>(id));
    break;
  case str2int("PathFinding"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPathFinding>(id));
    break;
  case str2int("PathToHeightmap"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPathToHeightmap>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("PathToHeightmapGaussian"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPathToHeightmapGaussian>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("PathToHeightmapPolygon"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPathToHeightmapPolygon>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("PathToHeightmapRange"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPathToHeightmapRange>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Peak"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPeak>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Perlin"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPerlin>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("PerlinBillow"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPerlinBillow>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("PingpongPerlin"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPingpongPerlin>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Plateau"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPlateau>(id));
    break;
  case str2int("Preview"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPreview>(id));
    break;
  case str2int("PreviewColorize"):
    this->add_node(std::make_shared<hesiod::vnode::ViewPreviewColorize>(id));
    break;
  case str2int("RecastCanyon"):
    this->add_node(std::make_shared<hesiod::vnode::ViewRecastCanyon>(id));
    break;
  case str2int("RecastPeak"):
    this->add_node(std::make_shared<hesiod::vnode::ViewRecastPeak>(id));
    break;
  case str2int("RecastRockySlopes"):
    this->add_node(std::make_shared<hesiod::vnode::ViewRecastRockySlopes>(id));
    break;
  case str2int("Recurve"):
    this->add_node(std::make_shared<hesiod::vnode::ViewRecurve>(id));
    break;
  case str2int("RecurveKura"):
    this->add_node(std::make_shared<hesiod::vnode::ViewRecurveKura>(id));
    break;
  case str2int("RecurveS"):
    this->add_node(std::make_shared<hesiod::vnode::ViewRecurveS>(id));
    break;
  case str2int("RelativeElevation"):
    this->add_node(std::make_shared<hesiod::vnode::ViewRelativeElevation>(id));
    break;
  case str2int("Remap"):
    this->add_node(std::make_shared<hesiod::vnode::ViewRemap>(id));
    break;
  case str2int("Rescale"):
    this->add_node(std::make_shared<hesiod::vnode::ViewRescale>(id));
    break;
  case str2int("RidgedPerlin"):
    this->add_node(std::make_shared<hesiod::vnode::ViewRidgedPerlin>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Rugosity"):
    this->add_node(std::make_shared<hesiod::vnode::ViewRugosity>(id));
    break;
  case str2int("SedimentDeposition"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSedimentDeposition>(id));
    break;
  case str2int("SelectBlobLog"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectBlobLog>(id));
    break;
  case str2int("SelectCavities"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectCavities>(id));
    break;
  case str2int("SelectElevationSlope"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectElevationSlope>(id));
    break;
  case str2int("SelectGradientNorm"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectGradientNorm>(id));
    break;
  case str2int("SelectEq"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectEq>(id));
    break;
  case str2int("SelectInterval"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectInterval>(id));
    break;
  case str2int("SelectPulse"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectPulse>(id));
    break;
  case str2int("SelectRivers"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectRivers>(id));
    break;
  case str2int("SelectTransitions"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSelectTransitions>(id));
    break;
  case str2int("Simplex"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSimplex>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Slope"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSlope>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("SmoothCpulse"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSmoothCpulse>(id));
    break;
  case str2int("SmoothFill"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSmoothFill>(id));
    break;
  case str2int("SmoothFillHoles"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSmoothFillHoles>(id));
    break;
  case str2int("SmoothFillSmearPeaks"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSmoothFillSmearPeaks>(id));
    break;
  case str2int("SteepenConvective"):
    this->add_node(std::make_shared<hesiod::vnode::ViewSteepenConvective>(id));
    break;
  case str2int("Step"):
    this->add_node(std::make_shared<hesiod::vnode::ViewStep>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("StratifyMultiscale"):
    this->add_node(std::make_shared<hesiod::vnode::ViewStratifyMultiscale>(id));
    break;
  case str2int("StratifyOblique"):
    this->add_node(std::make_shared<hesiod::vnode::ViewStratifyOblique>(id));
    break;
  case str2int("Thermal"):
    this->add_node(std::make_shared<hesiod::vnode::ViewThermal>(id));
    break;
  case str2int("ThermalAutoBedrock"):
    this->add_node(std::make_shared<hesiod::vnode::ViewThermalAutoBedrock>(id));
    break;
  case str2int("ThermalFlatten"):
    this->add_node(std::make_shared<hesiod::vnode::ViewThermalFlatten>(id));
    break;
  case str2int("ThermalScree"):
    this->add_node(std::make_shared<hesiod::vnode::ViewThermalScree>(id));
    break;
  case str2int("ToKernel"):
    this->add_node(std::make_shared<hesiod::vnode::ViewToKernel>(id));
    break;
  case str2int("ToMask"):
    this->add_node(std::make_shared<hesiod::vnode::ViewToMask>(id));
    break;
  case str2int("ValleyWidth"):
    this->add_node(std::make_shared<hesiod::vnode::ViewValleyWidth>(id));
    break;
  case str2int("ValueNoiseDelaunay"):
    this->add_node(std::make_shared<hesiod::vnode::ViewValueNoiseDelaunay>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("ValueNoiseLinear"):
    this->add_node(std::make_shared<hesiod::vnode::ViewValueNoiseLinear>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("ValueNoiseThinplate"):
    this->add_node(std::make_shared<hesiod::vnode::ViewValueNoiseThinplate>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("WaveDune"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWaveDune>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("WaveSine"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWaveSine>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("WaveSquare"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWaveSquare>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("WaveTriangular"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWaveTriangular>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Warp"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWarp>(id));
    break;
  case str2int("WarpDownslope"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWarpDownslope>(id));
    break;
  case str2int("White"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWhite>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("WhiteDensityMap"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWhiteDensityMap>(id));
    break;
  case str2int("WhiteSparse"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWhiteSparse>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Worley"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWorley>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("WorleyDouble"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWorleyDouble>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("WorleyPolyline"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWorleyPolyline>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("WorleyValue"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWorleyValue>(id, this->shape, this->tiling, this->overlap));
    break;
  case str2int("Wrinkle"):
    this->add_node(std::make_shared<hesiod::vnode::ViewWrinkle>(id));
    break;
  case str2int("ZeroedEdges"):
    this->add_node(std::make_shared<hesiod::vnode::ViewZeroedEdges>(id));
    break;
  default:
    // clang-format on
    {
      LOG_ERROR("unknown node type: [%s]", control_node_type.c_str());
      throw std::runtime_error("adding an unknown node type");
      break;
    }
  }

  return id;
}

} // namespace hesiod::vnode
