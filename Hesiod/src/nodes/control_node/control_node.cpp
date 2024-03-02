/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/attribute.hpp"
#include "hesiod/view_tree.hpp"
#include "macrologger.h"
#include "nlohmann/json_fwd.hpp"
#include <map>
#include <memory>
#include <vector>

#include "hesiod/control_node.hpp"

#define CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ClassName) bool ClassName ::serialize_json_v2(std::string fieldName, nlohmann::json& outputData) { return ControlNode::serialize_json_v2(fieldName, outputData); } \
                                                                bool ClassName ::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData) { return ControlNode::deserialize_json_v2(fieldName, inputData); }

namespace hesiod::cnode
{

bool ControlNode::serialize_json_v2(std::string fieldName, nlohmann::json& outputData) 
{
  std::vector<nlohmann::json> attributesListJsonData = {};

  for(std::map<std::string, std::unique_ptr<hesiod::Attribute>>::iterator currentAttributeIterator = attr.begin(); currentAttributeIterator != attr.end(); currentAttributeIterator++)
  {
    nlohmann::json currentAttributeIteratorJsonData = nlohmann::json();

    currentAttributeIteratorJsonData["type"] = AttributeInstancing::get_name_from_type(currentAttributeIterator->second->get_type());
    currentAttributeIteratorJsonData["key"] = currentAttributeIterator->first;
    currentAttributeIterator->second->serialize_json_v2("value", currentAttributeIteratorJsonData);
  
    attributesListJsonData.push_back(currentAttributeIteratorJsonData);
  }
  
  outputData[fieldName]["id"] = this->id;
  outputData[fieldName]["attributes"] = attributesListJsonData;
  return true;
}

bool ControlNode::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData)
{
  if(
    inputData[fieldName]["id"].is_string() == false ||
    inputData[fieldName]["attributes"].is_array() == false
  )
  {
    LOG_ERROR("Encountered invalid control node!");
    return false;
  }

  attr.clear();

  this->id = inputData[fieldName]["id"].get<std::string>();

  for(nlohmann::json currentAttributeIteratorJsonData : inputData[fieldName]["attributes"])
  {
    AttributeType currentAttributeIteratorType = AttributeInstancing::get_type_from_name(currentAttributeIteratorJsonData["type"].get<std::string>());

    if(currentAttributeIteratorType == AttributeType::INVALID)
    {
      LOG_ERROR("Encountered invalid control node attribute type!");
      continue;
    }

    std::unique_ptr<Attribute> currentAttribute = AttributeInstancing::create_attribute_from_type(currentAttributeIteratorType);
    std::string currentAttributeKey = currentAttributeIteratorJsonData["key"].get<std::string>();

    currentAttribute->deserialize_json_v2("value", currentAttributeIteratorJsonData);

    attr.emplace(currentAttributeKey, std::move(currentAttribute));
  }

  return true;
}

void ControlNode::post_process_heightmap(hmap::HeightMap &h)
{
  if (this->attr.contains("inverse"))
    if (GET_ATTR_BOOL("inverse"))
      h.inverse();

  if (this->attr.contains("smoothing"))
    if (GET_ATTR_BOOL("smoothing"))
    {
      int ir_smoothing = GET_ATTR_INT("ir_smoothing");

      hmap::transform(h,
                      [&ir_smoothing](hmap::Array &array)
                      { return hmap::smooth_cpulse(array, ir_smoothing); });
      h.smooth_overlap_buffers();
    }

  if (this->attr.contains("saturate"))
    if (GET_ATTR_REF_RANGE("saturate")->is_activated())
    {
      hmap::Vec2<float> srange = GET_ATTR_RANGE("saturate");
      float             k = GET_ATTR_FLOAT("k_saturate");

      float hmin = h.min();
      float hmax = h.max();

      // node parameters are assumed normalized and thus in [0, 1],
      // they need to be rescaled
      float smin_n = hmin + srange.x * (hmax - hmin);
      float smax_n = hmax - (1.f - srange.y) * (hmax - hmin);
      float k_n = k * (hmax - hmin);

      hmap::transform(h,
                      [&smin_n, &smax_n, &k_n](hmap::Array &array)
                      { hmap::clamp_smooth(array, smin_n, smax_n, k_n); });

      // keep original amplitude
      h.remap(hmin, hmax);
    }

  if (this->attr.contains("remap"))
    if (GET_ATTR_REF_RANGE("remap")->is_activated())
    {
      hmap::Vec2<float> vrange = GET_ATTR_RANGE("remap");
      h.remap(vrange.x, vrange.y);
    }
}

// ControlNodeInstancing

static std::map<ControlNodeType, std::string> control_node_type_to_name = {
 { ControlNodeType::UNARY, "Unary" },
 { ControlNodeType::BINARY, "Binary" },
 { ControlNodeType::DEBUG, "Debug" },
 { ControlNodeType::EROSION, "Erosion" },
 { ControlNodeType::FILTER, "Filter" },
 { ControlNodeType::MASK, "Mask" },
 { ControlNodeType::PRIMITIVE, "Primitive" },
 { ControlNodeType::CLONE, "Clone" },
 { ControlNodeType::ABS, "Abs" },
 { ControlNodeType::ABS_SMOOTH, "AbsSmooth" },
 { ControlNodeType::ALTER_ELEVATION, "AlterElevation" },
 { ControlNodeType::BASE_ELEVATION, "BaseElevation" },
 { ControlNodeType::BEZIER_PATH, "BezierPath" },
 { ControlNodeType::BIQUAD_PULSE, "BiquadPulse" },
 { ControlNodeType::BLEND, "Blend" },
 { ControlNodeType::BUMP, "Bump" },
 { ControlNodeType::BRUSH, "Brush" },
 { ControlNodeType::CALDERA, "Caldera" },
 { ControlNodeType::CHECKERBOARD, "Checkerboard" },
 { ControlNodeType::CLAMP, "Clamp" },
 { ControlNodeType::CLOUD, "Cloud" },
 { ControlNodeType::CLOUD_TO_ARRAY_INTERP, "CloudToArrayInterp" },
 { ControlNodeType::COLORIZE, "Colorize" },
 { ControlNodeType::COLORIZE_SOLID, "ColorizeSolid" },
 { ControlNodeType::COMBINE_MASK, "CombineMask" },
 { ControlNodeType::CONVOLVE_SVD, "ConvolveSVD" },
 { ControlNodeType::DENDRY, "Dendry" },
 { ControlNodeType::DEPRESSION_FILLING, "DepressionFilling" },
 { ControlNodeType::DIG_PATH, "DigPath" },
 { ControlNodeType::DISTANCE_TRANSFORM, "DistanceTransform" },
 { ControlNodeType::EQUALIZE, "Equalize" },
 { ControlNodeType::EROSION_MAPS, "ErosionMaps" },
 { ControlNodeType::EXPAND_SHRINK, "ExpandShrink" },
 { ControlNodeType::EXPAND_SHRINK_DIRECTIONAL, "ExpandShrinkDirectional" },
 { ControlNodeType::EXPORT, "Export" },
 { ControlNodeType::EXPORT_RGB, "ExportRGB" },
 { ControlNodeType::FACETED, "Faceted" },
 { ControlNodeType::FBM_IQ_PERLIN, "FbmIqPerlin" },
 { ControlNodeType::FBM_PERLIN, "FbmPerlin" },
 { ControlNodeType::FBM_SIMPLEX, "FbmSimplex" },
 { ControlNodeType::FBM_WORLEY, "FbmWorley" },
 { ControlNodeType::FBM_WORLEY_DOUBLE, "FbmWorleyDouble" },
 { ControlNodeType::FRACTALIZE_PATH, "FractalizePath" },
 { ControlNodeType::GABOR_NOISE, "GaborNoise" },
 { ControlNodeType::GAIN, "Gain" },
 { ControlNodeType::GAMMA_CORRECTION, "GammaCorrection" },
 { ControlNodeType::GAMMA_CORRECTION_LOCAL, "GammaCorrectionLocal" },
 { ControlNodeType::GAUSSIAN_PULSE, "GaussianPulse" },
 { ControlNodeType::GEOMORPHONS, "Geomorphons" },
 { ControlNodeType::GRADIENT, "Gradient" },
 { ControlNodeType::GRADIENT_ANGLE, "GradientAngle" },
 { ControlNodeType::GRADIENT_NORM, "GradientNorm" },
 { ControlNodeType::GRADIENT_TALUS, "GradientTalus" },
 { ControlNodeType::HYDRAULIC_ALGEBRIC, "HydraulicAlgebric" },
 { ControlNodeType::HYDRAULIC_PARTICLE, "HydraulicParticle" },
 { ControlNodeType::HYDRAULIC_RIDGE, "HydraulicRidge" },
 { ControlNodeType::HYDRAULIC_STREAM, "HydraulicStream" },
 { ControlNodeType::HYDRAULIC_STREAM_LOG, "HydraulicStreamLog" },
 { ControlNodeType::HYDRAULIC_VPIPES, "HydraulicVpipes" },
 { ControlNodeType::IMPORT, "Import" },
 { ControlNodeType::INVERSE, "Inverse" },
 { ControlNodeType::KERNEL, "Kernel" },
 { ControlNodeType::KMEANS_CLUSTERING2, "KmeansClustering2" },
 { ControlNodeType::KMEANS_CLUSTERING3, "KmeansClustering3" },
 { ControlNodeType::LAPLACE, "Laplace" },
 { ControlNodeType::LAPLACE_EDGE_PRESERVING, "LaplaceEdgePreserving" },
 { ControlNodeType::LERP, "Lerp" },
 { ControlNodeType::MAKE_BINARY, "MakeBinary" },
 { ControlNodeType::MEANDERIZE_PATH, "MeanderizePath" },
 { ControlNodeType::MEAN_LOCAL, "MeanLocal" },
 { ControlNodeType::MEDIAN3X3, "Median3x3" },
 { ControlNodeType::MINIMUM_LOCAL, "MinimumLocal" },
 { ControlNodeType::MIX_RGB, "MixRGB" },
 { ControlNodeType::NORMAL_DISPLACEMENT, "NormalDisplacement" },
 { ControlNodeType::ONE_MINUS, "OneMinus" },
 { ControlNodeType::PATH, "Path" },
 { ControlNodeType::PATH_FINDING, "PathFinding" },
 { ControlNodeType::PATH_TO_HEIGHTMAP, "PathToHeightmap" },
 { ControlNodeType::PEAK, "Peak" },
 { ControlNodeType::PERLIN, "Perlin" },
 { ControlNodeType::PERLIN_BILLOW, "PerlinBillow" },
 { ControlNodeType::PINGPONG_PERLIN, "PingpongPerlin" },
 { ControlNodeType::PLATEAU, "Plateau" },
 { ControlNodeType::PREVIEW, "Preview" },
 { ControlNodeType::PREVIEW_COLORIZE, "PreviewColorize" },
 { ControlNodeType::RECAST_CANYON, "RecastCanyon" },
 { ControlNodeType::RECAST_CLIFF, "RecastCliff" },
 { ControlNodeType::RECAST_CLIFF_DIRECTIONAL, "RecastCliffDirectional" },
 { ControlNodeType::RECAST_PEAK, "RecastPeak" },
 { ControlNodeType::RECAST_ROCKY_SLOPES, "RecastRockySlopes" },
 { ControlNodeType::RECURVE, "Recurve" },
 { ControlNodeType::RECURVE_KURA, "RecurveKura" },
 { ControlNodeType::RECURVE_S, "RecurveS" },
 { ControlNodeType::RELATIVE_ELEVATION, "RelativeElevation" },
 { ControlNodeType::REMAP, "Remap" },
 { ControlNodeType::RESCALE, "Rescale" },
 { ControlNodeType::RIDGED_PERLIN, "RidgedPerlin" },
 { ControlNodeType::RUGOSITY, "Rugosity" },
 { ControlNodeType::SEDIMENT_DEPOSITION, "SedimentDeposition" },
 { ControlNodeType::SELECT_BLOB_LOG, "SelectBlobLog" },
 { ControlNodeType::SELECT_CAVITIES, "SelectCavities" },
 { ControlNodeType::SELECT_EQ, "SelectEq" },
 { ControlNodeType::SELECT_ELEVATION_SLOPE, "SelectElevationSlope" },
 { ControlNodeType::SELECT_GRADIENT_NORM, "SelectGradientNorm" },
 { ControlNodeType::SELECT_INTERVAL, "SelectInterval" },
 { ControlNodeType::SELECT_PULSE, "SelectPulse" },
 { ControlNodeType::SELECT_RIVERS, "SelectRivers" },
 { ControlNodeType::SELECT_TRANSITIONS, "SelectTransitions" },
 { ControlNodeType::SIMPLEX, "Simplex" },
 { ControlNodeType::SLOPE, "Slope" },
 { ControlNodeType::SMOOTH_CPULSE, "SmoothCpulse" },
 { ControlNodeType::SMOOTH_FILL, "SmoothFill" },
 { ControlNodeType::SMOOTH_FILL_HOLES, "SmoothFillHoles" },
 { ControlNodeType::SMOOTH_FILL_SMEAR_PEAKS, "SmoothFillSmearPeaks" },
 { ControlNodeType::STEEPEN_CONVECTIVE, "SteepenConvective" },
 { ControlNodeType::STEP, "Step" },
 { ControlNodeType::STRATIFY_MULTISCALE, "StratifyMultiscale" },
 { ControlNodeType::STRATIFY_OBLIQUE, "StratifyOblique" },
 { ControlNodeType::THERMAL, "Thermal" },
 { ControlNodeType::THERMAL_AUTO_BEDROCK, "ThermalAutoBedrock" },
 { ControlNodeType::THERMAL_FLATTEN, "ThermalFlatten" },
 { ControlNodeType::THERMAL_SCREE, "ThermalScree" },
 { ControlNodeType::TO_KERNEL, "ToKernel" },
 { ControlNodeType::TO_MASK, "ToMask" },
 { ControlNodeType::VALLEY_WIDTH, "ValleyWidth" },
 { ControlNodeType::VALUE_NOISE_DELAUNAY, "ValueNoiseDelaunay" },
 { ControlNodeType::VALUE_NOISE_LINEAR, "ValueNoiseLinear" },
 { ControlNodeType::VALUE_NOISE_THINPLATE, "ValueNoiseThinplate" },
 { ControlNodeType::WARP, "Warp" },
 { ControlNodeType::WARP_DOWNSLOPE, "WarpDownslope" },
 { ControlNodeType::WAVE_DUNE, "WaveDune" },
 { ControlNodeType::WAVE_SINE, "WaveSine" },
 { ControlNodeType::WAVE_SQUARE, "WaveSquare" },
 { ControlNodeType::WAVE_TRIANGULAR, "WaveTriangular" },
 { ControlNodeType::WHITE, "White" },
 { ControlNodeType::WHITE_DENSITY_MAP, "WhiteDensityMap" },
 { ControlNodeType::WHITE_SPARSE, "WhiteSparse" },
 { ControlNodeType::WORLEY, "Worley" },
 { ControlNodeType::WORLEY_DOUBLE, "WorleyDouble" },
 { ControlNodeType::WORLEY_VALUE, "WorleyValue" },
 { ControlNodeType::WRINKLE, "Wrinkle" },
 { ControlNodeType::ZEROED_EDGES, "ZeroedEdges" }
};

std::string ControlNodeInstancing::get_name_from_type(ControlNodeType type)
{
    if(control_node_type_to_name.contains(type) == false)
    {
        LOG_ERROR("Encountered unknown type! (%i)", static_cast<int>(type));
        return "INVALID";
    }

    return control_node_type_to_name.at(type);
}

ControlNodeType ControlNodeInstancing::get_type_from_name(std::string name)
{
    for(std::map<ControlNodeType, std::string>::iterator currentIterator = control_node_type_to_name.begin(); currentIterator != control_node_type_to_name.end(); currentIterator++)
    {
        if(currentIterator->second == name)
        {
            return currentIterator->first;
        }
    }

    return ControlNodeType::INVALID;
}

std::unique_ptr<ControlNode> ControlNodeInstancing::create_attribute_from_type(ControlNodeType type, std::string id, void* tree_uncast)
{
  vnode::ViewTree* tree = static_cast<vnode::ViewTree*>(tree_uncast);

  switch (type) {
    default:
      case ControlNodeType::INVALID:
        LOG_ERROR("Encountered unknown type! (%i)", static_cast<int>(type));
        return nullptr;
      case ControlNodeType::UNARY: return std::unique_ptr<ControlNode>(new Unary(id));
      case ControlNodeType::BINARY: return std::unique_ptr<ControlNode>(new Binary(id));
      case ControlNodeType::DEBUG: return std::unique_ptr<ControlNode>(new Debug(id));
      case ControlNodeType::EROSION: return std::unique_ptr<ControlNode>(new Erosion(id));
      case ControlNodeType::FILTER: return std::unique_ptr<ControlNode>(new Filter(id));
      case ControlNodeType::MASK: return std::unique_ptr<ControlNode>(new Mask(id));
      case ControlNodeType::PRIMITIVE: return std::unique_ptr<ControlNode>(new Primitive(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::CLONE: return std::unique_ptr<ControlNode>(new Clone(id));
      case ControlNodeType::ABS: return std::unique_ptr<ControlNode>(new Abs(id));
      case ControlNodeType::ABS_SMOOTH: return std::unique_ptr<ControlNode>(new AbsSmooth(id));
      case ControlNodeType::ALTER_ELEVATION: return std::unique_ptr<ControlNode>(new AlterElevation(id));
      case ControlNodeType::BASE_ELEVATION: return std::unique_ptr<ControlNode>(new BaseElevation(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::BEZIER_PATH: return std::unique_ptr<ControlNode>(new BezierPath(id));
      case ControlNodeType::BIQUAD_PULSE: return std::unique_ptr<ControlNode>(new BiquadPulse(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::BLEND: return std::unique_ptr<ControlNode>(new Blend(id));
      case ControlNodeType::BUMP: return std::unique_ptr<ControlNode>(new Bump(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::BRUSH: return std::unique_ptr<ControlNode>(new Brush(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::CALDERA: return std::unique_ptr<ControlNode>(new Caldera(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::CHECKERBOARD: return std::unique_ptr<ControlNode>(new Checkerboard(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::CLAMP: return std::unique_ptr<ControlNode>(new Clamp(id));
      case ControlNodeType::CLOUD: return std::unique_ptr<ControlNode>(new Cloud(id));
      case ControlNodeType::CLOUD_TO_ARRAY_INTERP: return std::unique_ptr<ControlNode>(new CloudToArrayInterp(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::COLORIZE: return std::unique_ptr<ControlNode>(new Colorize(id));
      case ControlNodeType::COLORIZE_SOLID: return std::unique_ptr<ControlNode>(new ColorizeSolid(id));
      case ControlNodeType::COMBINE_MASK: return std::unique_ptr<ControlNode>(new CombineMask(id));
      case ControlNodeType::CONVOLVE_SVD: return std::unique_ptr<ControlNode>(new ConvolveSVD(id));
      case ControlNodeType::DENDRY: return std::unique_ptr<ControlNode>(new Dendry(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::DEPRESSION_FILLING: return std::unique_ptr<ControlNode>(new DepressionFilling(id));
      case ControlNodeType::DIG_PATH: return std::unique_ptr<ControlNode>(new DigPath(id));
      case ControlNodeType::DISTANCE_TRANSFORM: return std::unique_ptr<ControlNode>(new DistanceTransform(id));
      case ControlNodeType::EQUALIZE: return std::unique_ptr<ControlNode>(new Equalize(id));
      case ControlNodeType::EROSION_MAPS: return std::unique_ptr<ControlNode>(new ErosionMaps(id));
      case ControlNodeType::EXPAND_SHRINK: return std::unique_ptr<ControlNode>(new ExpandShrink(id));
      case ControlNodeType::EXPAND_SHRINK_DIRECTIONAL: return std::unique_ptr<ControlNode>(new ExpandShrinkDirectional(id));
      case ControlNodeType::EXPORT: return std::unique_ptr<ControlNode>(new Export(id));
      case ControlNodeType::EXPORT_RGB: return std::unique_ptr<ControlNode>(new ExportRGB(id));
      case ControlNodeType::FACETED: return std::unique_ptr<ControlNode>(new Faceted(id));
      case ControlNodeType::FBM_IQ_PERLIN: return std::unique_ptr<ControlNode>(new FbmIqPerlin(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::FBM_PERLIN: return std::unique_ptr<ControlNode>(new FbmPerlin(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::FBM_SIMPLEX: return std::unique_ptr<ControlNode>(new FbmSimplex(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::FBM_WORLEY: return std::unique_ptr<ControlNode>(new FbmWorley(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::FBM_WORLEY_DOUBLE: return std::unique_ptr<ControlNode>(new FbmWorleyDouble(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::FRACTALIZE_PATH: return std::unique_ptr<ControlNode>(new FractalizePath(id));
      case ControlNodeType::GABOR_NOISE: return std::unique_ptr<ControlNode>(new GaborNoise(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::GAIN: return std::unique_ptr<ControlNode>(new Gain(id));
      case ControlNodeType::GAMMA_CORRECTION: return std::unique_ptr<ControlNode>(new GammaCorrection(id));
      case ControlNodeType::GAMMA_CORRECTION_LOCAL: return std::unique_ptr<ControlNode>(new GammaCorrectionLocal(id));
      case ControlNodeType::GAUSSIAN_PULSE: return std::unique_ptr<ControlNode>(new GaussianPulse(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::GEOMORPHONS: return std::unique_ptr<ControlNode>(new Geomorphons(id));
      case ControlNodeType::GRADIENT: return std::unique_ptr<ControlNode>(new Gradient(id));
      case ControlNodeType::GRADIENT_ANGLE: return std::unique_ptr<ControlNode>(new GradientAngle(id));
      case ControlNodeType::GRADIENT_NORM: return std::unique_ptr<ControlNode>(new GradientNorm(id));
      case ControlNodeType::GRADIENT_TALUS: return std::unique_ptr<ControlNode>(new GradientTalus(id));
      case ControlNodeType::HYDRAULIC_ALGEBRIC: return std::unique_ptr<ControlNode>(new HydraulicAlgebric(id));
      case ControlNodeType::HYDRAULIC_PARTICLE: return std::unique_ptr<ControlNode>(new HydraulicParticle(id));
      case ControlNodeType::HYDRAULIC_RIDGE: return std::unique_ptr<ControlNode>(new HydraulicRidge(id));
      case ControlNodeType::HYDRAULIC_STREAM: return std::unique_ptr<ControlNode>(new HydraulicStream(id));
      case ControlNodeType::HYDRAULIC_STREAM_LOG: return std::unique_ptr<ControlNode>(new HydraulicStreamLog(id));
      case ControlNodeType::HYDRAULIC_VPIPES: return std::unique_ptr<ControlNode>(new HydraulicVpipes(id));
      case ControlNodeType::IMPORT: return std::unique_ptr<ControlNode>(new Import(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::INVERSE: return std::unique_ptr<ControlNode>(new Inverse(id));
      case ControlNodeType::KERNEL: return std::unique_ptr<ControlNode>(new Kernel(id));
      case ControlNodeType::KMEANS_CLUSTERING2: return std::unique_ptr<ControlNode>(new KmeansClustering2(id));
      case ControlNodeType::KMEANS_CLUSTERING3: return std::unique_ptr<ControlNode>(new KmeansClustering3(id));
      case ControlNodeType::LAPLACE: return std::unique_ptr<ControlNode>(new Laplace(id));
      case ControlNodeType::LAPLACE_EDGE_PRESERVING: return std::unique_ptr<ControlNode>(new LaplaceEdgePreserving(id));
      case ControlNodeType::LERP: return std::unique_ptr<ControlNode>(new Lerp(id));
      case ControlNodeType::MAKE_BINARY: return std::unique_ptr<ControlNode>(new MakeBinary(id));
      case ControlNodeType::MEANDERIZE_PATH: return std::unique_ptr<ControlNode>(new MeanderizePath(id));
      case ControlNodeType::MEAN_LOCAL: return std::unique_ptr<ControlNode>(new MeanLocal(id));
      case ControlNodeType::MEDIAN3X3: return std::unique_ptr<ControlNode>(new Median3x3(id));
      case ControlNodeType::MINIMUM_LOCAL: return std::unique_ptr<ControlNode>(new MinimumLocal(id));
      case ControlNodeType::MIX_RGB: return std::unique_ptr<ControlNode>(new MixRGB(id));
      case ControlNodeType::NORMAL_DISPLACEMENT: return std::unique_ptr<ControlNode>(new NormalDisplacement(id));
      case ControlNodeType::ONE_MINUS: return std::unique_ptr<ControlNode>(new OneMinus(id));
      case ControlNodeType::PATH: return std::unique_ptr<ControlNode>(new Path(id));
      case ControlNodeType::PATH_FINDING: return std::unique_ptr<ControlNode>(new PathFinding(id));
      case ControlNodeType::PATH_TO_HEIGHTMAP: return std::unique_ptr<ControlNode>(new PathToHeightmap(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::PEAK: return std::unique_ptr<ControlNode>(new Peak(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::PERLIN: return std::unique_ptr<ControlNode>(new Perlin(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::PERLIN_BILLOW: return std::unique_ptr<ControlNode>(new PerlinBillow(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::PINGPONG_PERLIN: return std::unique_ptr<ControlNode>(new PingpongPerlin(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::PLATEAU: return std::unique_ptr<ControlNode>(new Plateau(id));
      case ControlNodeType::PREVIEW: return std::unique_ptr<ControlNode>(new Preview(id));
      case ControlNodeType::PREVIEW_COLORIZE: return std::unique_ptr<ControlNode>(new PreviewColorize(id));
      case ControlNodeType::RECAST_CANYON: return std::unique_ptr<ControlNode>(new RecastCanyon(id));
      case ControlNodeType::RECAST_CLIFF: return std::unique_ptr<ControlNode>(new RecastCliff(id));
      case ControlNodeType::RECAST_CLIFF_DIRECTIONAL: return std::unique_ptr<ControlNode>(new RecastCliffDirectional(id));
      case ControlNodeType::RECAST_PEAK: return std::unique_ptr<ControlNode>(new RecastPeak(id));
      case ControlNodeType::RECAST_ROCKY_SLOPES: return std::unique_ptr<ControlNode>(new RecastRockySlopes(id));
      case ControlNodeType::RECURVE: return std::unique_ptr<ControlNode>(new Recurve(id));
      case ControlNodeType::RECURVE_KURA: return std::unique_ptr<ControlNode>(new RecurveKura(id));
      case ControlNodeType::RECURVE_S: return std::unique_ptr<ControlNode>(new RecurveS(id));
      case ControlNodeType::RELATIVE_ELEVATION: return std::unique_ptr<ControlNode>(new RelativeElevation(id));
      case ControlNodeType::REMAP: return std::unique_ptr<ControlNode>(new Remap(id));
      case ControlNodeType::RESCALE: return std::unique_ptr<ControlNode>(new Rescale(id));
      case ControlNodeType::RIDGED_PERLIN: return std::unique_ptr<ControlNode>(new RidgedPerlin(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::RUGOSITY: return std::unique_ptr<ControlNode>(new Rugosity(id));
      case ControlNodeType::SEDIMENT_DEPOSITION: return std::unique_ptr<ControlNode>(new SedimentDeposition(id));
      case ControlNodeType::SELECT_BLOB_LOG: return std::unique_ptr<ControlNode>(new SelectBlobLog(id));
      case ControlNodeType::SELECT_CAVITIES: return std::unique_ptr<ControlNode>(new SelectCavities(id));
      case ControlNodeType::SELECT_EQ: return std::unique_ptr<ControlNode>(new SelectEq(id));
      case ControlNodeType::SELECT_ELEVATION_SLOPE: return std::unique_ptr<ControlNode>(new SelectElevationSlope(id));
      case ControlNodeType::SELECT_GRADIENT_NORM: return std::unique_ptr<ControlNode>(new SelectGradientNorm(id));
      case ControlNodeType::SELECT_INTERVAL: return std::unique_ptr<ControlNode>(new SelectInterval(id));
      case ControlNodeType::SELECT_PULSE: return std::unique_ptr<ControlNode>(new SelectPulse(id));
      case ControlNodeType::SELECT_RIVERS: return std::unique_ptr<ControlNode>(new SelectRivers(id));
      case ControlNodeType::SELECT_TRANSITIONS: return std::unique_ptr<ControlNode>(new SelectTransitions(id));
      case ControlNodeType::SIMPLEX: return std::unique_ptr<ControlNode>(new Simplex(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::SLOPE: return std::unique_ptr<ControlNode>(new Slope(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::SMOOTH_CPULSE: return std::unique_ptr<ControlNode>(new SmoothCpulse(id));
      case ControlNodeType::SMOOTH_FILL: return std::unique_ptr<ControlNode>(new SmoothFill(id));
      case ControlNodeType::SMOOTH_FILL_HOLES: return std::unique_ptr<ControlNode>(new SmoothFillHoles(id));
      case ControlNodeType::SMOOTH_FILL_SMEAR_PEAKS: return std::unique_ptr<ControlNode>(new SmoothFillSmearPeaks(id));
      case ControlNodeType::STEEPEN_CONVECTIVE: return std::unique_ptr<ControlNode>(new SteepenConvective(id));
      case ControlNodeType::STEP: return std::unique_ptr<ControlNode>(new Step(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::STRATIFY_MULTISCALE: return std::unique_ptr<ControlNode>(new StratifyMultiscale(id));
      case ControlNodeType::STRATIFY_OBLIQUE: return std::unique_ptr<ControlNode>(new StratifyOblique(id));
      case ControlNodeType::THERMAL: return std::unique_ptr<ControlNode>(new Thermal(id));
      case ControlNodeType::THERMAL_AUTO_BEDROCK: return std::unique_ptr<ControlNode>(new ThermalAutoBedrock(id));
      case ControlNodeType::THERMAL_FLATTEN: return std::unique_ptr<ControlNode>(new ThermalFlatten(id));
      case ControlNodeType::THERMAL_SCREE: return std::unique_ptr<ControlNode>(new ThermalScree(id));
      case ControlNodeType::TO_KERNEL: return std::unique_ptr<ControlNode>(new ToKernel(id));
      case ControlNodeType::TO_MASK: return std::unique_ptr<ControlNode>(new ToMask(id));
      case ControlNodeType::VALLEY_WIDTH: return std::unique_ptr<ControlNode>(new ValleyWidth(id));
      case ControlNodeType::VALUE_NOISE_DELAUNAY: return std::unique_ptr<ControlNode>(new ValueNoiseDelaunay(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::VALUE_NOISE_LINEAR: return std::unique_ptr<ControlNode>(new ValueNoiseLinear(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::VALUE_NOISE_THINPLATE: return std::unique_ptr<ControlNode>(new ValueNoiseThinplate(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::WARP: return std::unique_ptr<ControlNode>(new Warp(id));
      case ControlNodeType::WARP_DOWNSLOPE: return std::unique_ptr<ControlNode>(new WarpDownslope(id));
      case ControlNodeType::WAVE_DUNE: return std::unique_ptr<ControlNode>(new WaveDune(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::WAVE_SINE: return std::unique_ptr<ControlNode>(new WaveSine(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::WAVE_SQUARE: return std::unique_ptr<ControlNode>(new WaveSquare(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::WAVE_TRIANGULAR: return std::unique_ptr<ControlNode>(new WaveTriangular(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::WHITE: return std::unique_ptr<ControlNode>(new White(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::WHITE_DENSITY_MAP: return std::unique_ptr<ControlNode>(new WhiteDensityMap(id));
      case ControlNodeType::WHITE_SPARSE: return std::unique_ptr<ControlNode>(new WhiteSparse(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::WORLEY: return std::unique_ptr<ControlNode>(new Worley(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::WORLEY_DOUBLE: return std::unique_ptr<ControlNode>(new WorleyDouble(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::WORLEY_VALUE: return std::unique_ptr<ControlNode>(new WorleyValue(id, tree->get_shape(), tree->get_tiling(), tree->get_overlap()));
      case ControlNodeType::WRINKLE: return std::unique_ptr<ControlNode>(new Wrinkle(id));
      case ControlNodeType::ZEROED_EDGES: return std::unique_ptr<ControlNode>(new ZeroedEdges(id));
  }
}

// Generic nodes

CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Unary);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Binary);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Debug);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Erosion);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Filter);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Mask);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Primitive);

// Data routing nodes

bool Clone::serialize_json_v2(std ::string fieldName, nlohmann ::json &outputData) 
{
  if(ControlNode::serialize_json_v2("base", outputData[fieldName]) == false)
  {
    return false;
  }

  std::vector<std::string> output_ids = {};

  this->update_inner_bindings();

  for (auto& [port_id, port] : this->get_ports())
  {
    if (port.direction == gnode::direction::out)
    {
      output_ids.push_back(port_id.c_str());
    }
  }

  outputData[fieldName]["output_ids"] = output_ids;
  outputData[fieldName]["id_count"] = id_count;

  return true;
}

bool Clone::deserialize_json_v2(std ::string fieldName, nlohmann ::json &inputData) 
{
  if(
    inputData[fieldName]["base"].is_object() == false ||
    inputData[fieldName]["output_ids"].is_array() == false ||
    inputData[fieldName]["id_count"].is_number() == false
  )
  {
    LOG_ERROR("Encountered invalid Clone ControlNode!");
    return false;
  }

  if(ControlNode::deserialize_json_v2("base", inputData[fieldName]) == false)
  {
    return false;
  }

  this->update_inner_bindings();

  std::vector<std::string> output_ids = inputData[fieldName]["output_ids"].get<std::vector<std::string>>();

  for (auto &port_id : output_ids)
  {
    if (!this->is_port_id_in_keys(port_id))
    {
      this->add_port(gnode::Port(port_id, gnode::direction::out, hesiod::cnode::dtype::dHeightMap));
    }
  }

  id_count = inputData[fieldName]["id_count"].get<int>();

  this->update_inner_bindings();

  return true;
}

// End-user nodes

CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Abs);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(AbsSmooth);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(AlterElevation);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(BaseElevation);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(BezierPath);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(BiquadPulse);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Blend);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Bump);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Brush);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Caldera);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Checkerboard);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Clamp);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Cloud);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(CloudToArrayInterp);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Colorize);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ColorizeSolid);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(CombineMask);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ConvolveSVD);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Dendry);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(DepressionFilling);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(DigPath);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(DistanceTransform);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Equalize);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ErosionMaps);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ExpandShrink);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ExpandShrinkDirectional);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Export);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ExportRGB);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Faceted);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(FbmIqPerlin);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(FbmPerlin);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(FbmSimplex);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(FbmWorley);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(FbmWorleyDouble);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(FractalizePath);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(GaborNoise);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Gain);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(GammaCorrection);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(GammaCorrectionLocal);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(GaussianPulse);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Geomorphons);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Gradient);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(GradientAngle);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(GradientNorm);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(GradientTalus);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(HydraulicAlgebric);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(HydraulicParticle);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(HydraulicRidge);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(HydraulicStream);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(HydraulicStreamLog);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(HydraulicVpipes);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Import);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Inverse);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Kernel);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(KmeansClustering2);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(KmeansClustering3);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Laplace);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(LaplaceEdgePreserving);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Lerp);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(MakeBinary);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(MeanderizePath);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(MeanLocal);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Median3x3);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(MinimumLocal);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(MixRGB);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(NormalDisplacement);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(OneMinus);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Path);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(PathFinding);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(PathToHeightmap);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Peak);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Perlin);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(PerlinBillow);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(PingpongPerlin);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Plateau);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Preview);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(PreviewColorize);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(RecastCanyon);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(RecastCliff);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(RecastCliffDirectional);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(RecastPeak);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(RecastRockySlopes);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Recurve);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(RecurveKura);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(RecurveS);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(RelativeElevation);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Remap);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Rescale);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(RidgedPerlin);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Rugosity);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SedimentDeposition);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SelectBlobLog);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SelectCavities);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SelectEq);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SelectElevationSlope);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SelectGradientNorm);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SelectInterval);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SelectPulse);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SelectRivers);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SelectTransitions);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Simplex);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Slope);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SmoothCpulse);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SmoothFill);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SmoothFillHoles);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SmoothFillSmearPeaks);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(SteepenConvective);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Step);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(StratifyMultiscale);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(StratifyOblique);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Thermal);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ThermalAutoBedrock);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ThermalFlatten);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ThermalScree);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ToKernel);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ToMask);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ValleyWidth);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ValueNoiseDelaunay);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ValueNoiseLinear);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ValueNoiseThinplate);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Warp);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(WarpDownslope);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(WaveDune);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(WaveSine);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(WaveSquare);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(WaveTriangular);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(White);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(WhiteDensityMap);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(WhiteSparse);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Worley);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(WorleyDouble);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(WorleyValue);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Wrinkle);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ZeroedEdges);

} // namespace hesiod::cnode
