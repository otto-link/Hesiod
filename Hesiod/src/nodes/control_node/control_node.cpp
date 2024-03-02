/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/attribute.hpp"
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
