/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "gnode.hpp"

#include "hesiod/attribute.hpp"
#include "hesiod/serialization.hpp"

// clang-format off
#define DEFAULT_KERNEL_SHAPE {17, 17}
#define DEFAULT_KW 2.f
#define DEFAULT_SEED 1

#define CAST_PORT_REF(type, port_id)  static_cast<type *>(this->get_p_data(port_id))

namespace hesiod::cnode
{

// define hesiod's own data types
enum dtype : int
{
  dArray,
  dCloud,
  dHeightMap,
  dHeightMapRGB,
  dPath
};

enum blending_method : int
{
  add,
  exclusion,
  gradients,
  maximum,
  maximum_smooth,
  minimum,
  minimum_smooth,
  multiply,
  multiply_add,
  negate,
  overlay,
  soft,
  substract
};

enum export_type : int
{
  binary,
  png8bit,
  png16bit,
  raw16bit
};

enum kernel : int
{
  cone,
  cubic_pulse,
  lorentzian,
  smooth_cosine
};

//----------------------------------------
// Node types / categories
//----------------------------------------

static const std::map<std::string, std::string> category_mapping = {
    {"Abs", "Math/Base"},
    {"AbsSmooth", "Math/Base"},
    // {"AlterElevation", "Operator/Transform"}, // BROKEN
    {"BaseElevation", "Primitive/Manual"},
    {"BezierPath", "Geometry/Path"},
    {"BiquadPulse", "Primitive/Function"},
    {"Blend", "Operator/Blend"},
    {"Brush", "Primitive/Manual"},
    {"Bump", "Primitive/Function"},
    {"Caldera", "Primitive/Geological"},
    {"Checkerboard", "Primitive/Coherent Noise"},
    {"Clamp", "Filter/Range"},
    {"Clone", "Routing"},
    {"Cloud", "Geometry/Cloud"},
    {"CloudToArrayInterp", "Primitive/Manual"},
    {"Colorize", "Texture"},
    {"ColorizeSolid", "Texture"},
    {"CombineMask", "Mask"},
    {"PreviewColorize", "Texture"},
    {"ConvolveSVD", "Math/Convolution"},
    {"Debug", "Debug"},
    {"Dendry", "Primitive/Coherent"},
    {"DepressionFilling", "Erosion"}, // not distributed
    {"DigPath", "Roads"},             // partially distributed
    {"DistanceTransform", "Math"},
    {"Equalize", "Filter/Recurve"}, // not distributed
    {"ErosionMaps", "Erosion/Hydraulic"},
    {"ExpandShrink", "Filter/Recast"},
    {"ExpandShrinkDirectional", "Filter/Recast"},
    {"Export", "IO/Files"},
    {"ExportRGB", "IO/Files"},
    {"Faceted", "Filter/Recast"}, // not distributed
    {"FbmIqPerlin", "Primitive/Coherent Noise"},
    {"FbmPerlin", "Primitive/Coherent Noise"},
    {"FbmSimplex", "Primitive/Coherent Noise"},
    {"FbmWorley", "Primitive/Coherent Noise"},
    {"FbmWorleyDouble", "Primitive/Coherent Noise"},
    {"FractalizePath", "Geometry/Path"},
    {"GaborNoise", "Primitive/Coherent Noise"},
    {"Gain", "Filter/Recurve"},
    {"GammaCorrection", "Filter/Recurve"},
    {"GammaCorrectionLocal", "Filter/Recurve"},
    {"GaussianPulse", "Primitive/Function"},
    {"Geomorphons", "Features"},
    {"Gradient", "Math/Gradient"},
    {"GradientAngle", "Math/Gradient"},
    {"GradientNorm", "Math/Gradient"},
    {"GradientTalus", "Math/Gradient"},
    {"ToKernel", "Primitive/Kernel"},
    {"HydraulicAlgebric", "Erosion/Hydraulic"},
    {"HydraulicParticle", "Erosion/Hydraulic"},
    {"HydraulicRidge", "Erosion/Hydraulic"}, // not distributed
    {"HydraulicStream", "Erosion/Hydraulic"},
    {"HydraulicStreamLog", "Erosion/Hydraulic"},
    {"HydraulicVpipes", "Erosion/Hydraulic"},
    {"Import", "IO/Files"},
    {"Inverse", "Math/Base"},
    {"Kernel", "Primitive/Kernel"},
    {"KmeansClustering2", "Features"}, // not distributed
    {"KmeansClustering3", "Features"}, // not distributed
    {"Laplace", "Filter/Smoothing"},
    {"LaplaceEdgePreserving", "Filter/Smoothing"},
    {"Lerp", "Operator/Blend"},
    {"MakeBinary", "Filter/Recurve"},
    {"MeanderizePath", "Geometry/Path"},
    {"MeanLocal", "Filter/Smoothing"},
    {"Median3x3", "Filter/Smoothing"},
    {"MinimumLocal", "Filter/Smoothing"},
    {"MixRGB", "Texture"},
    {"NormalDisplacement", "Filter/Recast"},
    {"OneMinus", "Math/Base"},
    {"Path", "Geometry/Path"},
    {"PathFinding", "Roads"},
    {"PathToHeightmap", "Geometry/Path"},
    {"Peak", "Primitive/Geological"},
    {"Perlin", "Primitive/Coherent Noise"},
    {"PerlinBillow", "Primitive/Coherent Noise"},
    {"PingpongPerlin", "Primitive/Coherent Noise"},
    {"Plateau", "Filter/Recurve"},
    {"Preview", "Debug"},
    {"RecastCanyon", "Filter/Recast"},
    {"RecastCliff", "Filter/Recast"},
    {"RecastCliffDirectional", "Filter/Recast"},
    {"RecastPeak", "Filter/Recast"},
    {"RecastRockySlopes", "Filter/Recast"},
    {"Recurve", "Filter/Recurve"},
    {"RecurveKura", "Filter/Recurve"},
    {"RecurveS", "Filter/Recurve"},
    {"RelativeElevation", "Features"},
    {"Remap", "Filter/Range"},
    {"Rescale", "Filter/Range"},
    {"RidgedPerlin", "Primitive/Coherent Noise"},
    {"Rugosity", "Features"},
    {"SedimentDeposition", "Erosion/Thermal"},
    {"SelectBlobLog", "Mask"},
    {"SelectCavities", "Mask"},
    {"SelectElevationSlope", "Mask"},
    {"SelectEq", "Mask"},
    {"SelectGradientNorm", "Mask"},
    {"SelectInterval", "Mask"},
    {"SelectPulse", "Mask"},
    {"SelectRivers", "Mask"}, // not distributed
    {"SelectTransitions", "Mask"},
    {"Simplex", "Primitive/Coherent Noise"},
    {"Slope", "Primitive/Function"},
    {"SmoothCpulse", "Filter/Smoothing"},
    {"SmoothFill", "Filter/Smoothing"},
    {"SmoothFillHoles", "Filter/Smoothing"},
    {"SmoothFillSmearPeaks", "Filter/Smoothing"},
    {"SteepenConvective", "Filter/Recast"},
    {"Step", "Primitive/Function"},
    {"StratifyMultiscale", "Erosion/Stratify"},
    {"StratifyOblique", "Erosion/Stratify"},
    {"Thermal", "Erosion/Thermal"},
    {"ThermalAutoBedrock", "Erosion/Thermal"},
    {"ThermalFlatten", "Erosion/Thermal"},
    {"ThermalScree", "Erosion/Thermal"},
    {"ToMask", "Mask"},
    {"ValleyWidth", "Features"},
    {"ValueNoiseDelaunay", "Primitive/Coherent Noise"},
    {"ValueNoiseLinear", "Primitive/Coherent Noise"},
    {"ValueNoiseThinplate", "Primitive/Coherent Noise"},
    {"Warp", "Operator/Transform"},
    {"WarpDownslope", "Operator/Transform"},
    {"WaveDune", "Primitive/Function"},
    {"WaveSine", "Primitive/Function"},
    {"WaveSquare", "Primitive/Function"},
    {"WaveTriangular", "Primitive/Function"},
    {"White", "Primitive/Random"},
    {"WhiteDensityMap", "Primitive/Random"},
    {"WhiteSparse", "Primitive/Random"},
    {"Worley", "Primitive/Coherent Noise"},
    {"WorleyDouble", "Primitive/Coherent Noise"},
    {"WorleyValue", "Primitive/Coherent Noise"},
    {"Wrinkle", "Filter/Recast"},
    {"ZeroedEdges", "Math/Boundaries"}};

//----------------------------------------
// Base node class
//----------------------------------------

class ControlNode : public gnode::Node, serialization::SerializationBase
{
public:
  std::map<std::string, std::unique_ptr<hesiod::Attribute>> attr = {};
  std::vector<std::string> attr_ordered_key = {};

  ControlNode() : gnode::Node()
  {
  }

  ControlNode(std::string id) : gnode::Node(id)
  {
  }

  bool serialize_json_v2(std::string field_name, nlohmann::json& output_data);

  bool deserialize_json_v2(std::string field_name, nlohmann::json& input_data);

  void post_process_heightmap(hmap::HeightMap &h);
};

//----------------------------------------
// Generic nodes
//----------------------------------------

class Unary : virtual public ControlNode // most basic, 1 in / 1 out
{
public:
  Unary(std::string id);

  void update_inner_bindings();

  void compute();

  virtual void compute_in_out(hmap::HeightMap &, hmap::HeightMap *)
  {
    LOG_ERROR("Compute not defined for generic in/out [%s])", this->id.c_str());
    throw std::runtime_error("undefined 'compute_in_out' method");
  }

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Binary : virtual public ControlNode // basic, 2 in / 1 out
{
public:
  Binary(std::string id);

  void update_inner_bindings();

  void compute();

  virtual void compute_in_out(hmap::HeightMap &,
                              hmap::HeightMap *,
                              hmap::HeightMap *)
  {
    LOG_ERROR("Compute not defined for generic in/out [%s])", this->id.c_str());
    throw std::runtime_error("undefined 'compute_in_out' method");
  }

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Debug : virtual public ControlNode
{
public:
  Debug(std::string id);

  void compute();

};

class Erosion : virtual public ControlNode
{
public:
  Erosion(std::string id);

  void update_inner_bindings();

  void compute();

  virtual void compute_erosion(hmap::HeightMap &, // eroded hmap
                               hmap::HeightMap *, // bedrock
                               hmap::HeightMap *, // moisture map
                               hmap::HeightMap *, // mask
                               hmap::HeightMap *, // erosion map
                               hmap::HeightMap *) // deposition map
  {
    LOG_ERROR("Compute not defined for generic filter [%s]", this->id.c_str());
    throw std::runtime_error("undefined 'compute_erosion' method");
  }

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap erosion_map = hmap::HeightMap();
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class Filter : virtual public ControlNode
{
public:
  Filter(std::string id);

  void update_inner_bindings();

  void compute();

  virtual void compute_filter(hmap::HeightMap &, hmap::HeightMap *)
  {
    LOG_ERROR("Compute not defined for generic filter [%s])", this->id.c_str());
    throw std::runtime_error("undefined 'compute_filter' method");
  }

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Mask : virtual public ControlNode
{
public:
  Mask(std::string id);

  void update_inner_bindings();

  void compute();

  virtual void compute_mask(hmap::HeightMap &, hmap::HeightMap *)
  {
    LOG_ERROR("Compute not defined for generic mask [%s])", this->id.c_str());
    throw std::runtime_error("undefined 'compute_mask' method");
  }

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Primitive : virtual public ControlNode
{
public:
  Primitive(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  void update_inner_bindings();

  void post_compute();


protected:
  hmap::HeightMap value_out = hmap::HeightMap();

 

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

//----------------------------------------
// Data routing nodes
//----------------------------------------

class Clone : virtual public ControlNode
{
public:
  Clone(std::string id);

  std::string add_thru_port();

  void remove_unused_outputs();

  void update_inner_bindings();

  void compute();


  bool serialize_json_v2(std::string field_name, nlohmann::json& output_data) override;
  bool deserialize_json_v2(std::string field_name, nlohmann::json& input_data) override;




protected:
  int             id_count = 0;
  int             n_outputs = 1;
  hmap::HeightMap value_out = hmap::HeightMap();
};

//----------------------------------------
// End-user nodes
//----------------------------------------

class Abs : public Unary
{
public:
  Abs(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class AbsSmooth : virtual public ControlNode
{
public:
  AbsSmooth(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class AlterElevation : virtual public ControlNode
{
public:
  AlterElevation(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class BaseElevation : public Primitive
{
public:
  BaseElevation(std::string     id,
                hmap::Vec2<int> shape,
                hmap::Vec2<int> tiling,
                float           overlap);

  void compute();

};

class BezierPath : virtual public ControlNode
{
public:
  BezierPath(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
};

class BiquadPulse : public Primitive
{
public:
  BiquadPulse(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  void compute();

};

class Blend : public Binary
{
public:
  Blend(std::string id);

  void compute_in_out(hmap::HeightMap &h_out,
                      hmap::HeightMap *p_h_in1,
                      hmap::HeightMap *p_h_in2);

 

private:
  std::map<std::string, int> blending_method_map = {
      {"add", blending_method::add},
      {"exclusion", blending_method::exclusion},
      {"gradients", blending_method::gradients},
      {"maximum", blending_method::maximum},
      {"maximum_smooth", blending_method::maximum_smooth},
      {"minimum", blending_method::minimum},
      {"minimum_smooth", blending_method::minimum_smooth},
      {"multiply", blending_method::multiply},
      {"multiply_add", blending_method::multiply_add},
      {"negate", blending_method::negate},
      {"overlay", blending_method::overlay},
      {"soft", blending_method::soft},
      {"substract", blending_method::substract}};
};

class Bump : public Primitive
{
public:
  Bump(std::string     id,
       hmap::Vec2<int> shape,
       hmap::Vec2<int> tiling,
       float           overlap);

  void compute();

};

// TO DO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class Brush : virtual public ControlNode
{
public:
  Brush(std::string     id,
        hmap::Vec2<int> shape,
        hmap::Vec2<int> tiling,
        float           overlap);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           vmin = 0.f;
  float           vmax = 1.f;
  bool            inverse = false;
  bool            remap = false;

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class Caldera : public Primitive
{
public:
  Caldera(std::string     id,
          hmap::Vec2<int> shape,
          hmap::Vec2<int> tiling,
          float           overlap);

  void compute();

};

class Checkerboard : public Primitive
{
public:
  Checkerboard(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

};

class Clamp : public Unary
{
public:
  Clamp(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class Cloud : virtual public ControlNode
{
public:
  Cloud(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Cloud value_out = hmap::Cloud();
};

class CloudToArrayInterp : public Primitive
{
public:
  CloudToArrayInterp(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap);

  void compute();

};

class Colorize : virtual public ControlNode
{
public:
  Colorize(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMapRGB value_out = hmap::HeightMapRGB();
};

class ColorizeSolid : virtual public ControlNode
{
public:
  ColorizeSolid(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMapRGB value_out = hmap::HeightMapRGB();
};

class CombineMask : virtual public ControlNode
{
public:
  CombineMask(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap            value_out = hmap::HeightMap();
  std::map<std::string, int> method_map = {{"union", 0},
                                           {"intersection", 1},
                                           {"exclusion", 2}};
};

class ConvolveSVD : virtual public ControlNode
{
public:
  ConvolveSVD(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Dendry : virtual public ControlNode
{
public:
  Dendry(std::string     id,
         hmap::Vec2<int> shape,
         hmap::Vec2<int> tiling,
         float           overlap);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class DepressionFilling : public Unary
{
public:
  DepressionFilling(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class DigPath : virtual public ControlNode
{
public:
  DigPath(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class DistanceTransform : virtual public ControlNode
{
public:
  DistanceTransform(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Equalize : public Filter
{
public:
  Equalize(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class ErosionMaps : virtual public ControlNode
{
public:
  ErosionMaps(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap erosion_map = hmap::HeightMap();
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class ExpandShrink : public Filter
{
public:
  ExpandShrink(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  std::map<std::string, int> kernel_map = {
      {"cone", kernel::cone},
      {"cubic_pulse", kernel::cubic_pulse},
      {"lorentzian", kernel::lorentzian},
      {"smooth_cosine", kernel::smooth_cosine}};
};

class ExpandShrinkDirectional : public Filter
{
public:
  ExpandShrinkDirectional(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class Export : virtual public ControlNode
{
public:
  Export(std::string id);

  void compute();

  void write_file();

protected:
  std::map<std::string, int> format_map = {
      // {"binary", hesiod::cnode::export_type::binary},
      {"png (8 bit)", hesiod::cnode::export_type::png8bit},
      {"png (16 bit)", hesiod::cnode::export_type::png16bit},
      {"raw (16 bit, Unity)", hesiod::cnode::export_type::raw16bit}};
};

class ExportRGB : virtual public ControlNode
{
public:
  ExportRGB(std::string id);

  void compute();

  void write_file();

};

class Faceted : virtual public ControlNode
{
public:
  Faceted(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();

private:
  std::map<std::string, int> neighborhood_map = {
      {"Moore", hmap::neighborhood::moore},
      {"Von Neumann", hmap::neighborhood::von_neumann},
      {"cross", hmap::neighborhood::cross}};
};

class FbmIqPerlin : public Primitive
{
public:
  FbmIqPerlin(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  void compute();

};

class FbmPerlin : public Primitive
{
public:
  FbmPerlin(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  void compute();

};

class FbmSimplex : public Primitive
{
public:
  FbmSimplex(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  void compute();

};

class FbmWorley : public Primitive
{
public:
  FbmWorley(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  void compute();

};

class FbmWorleyDouble : public Primitive
{
public:
  FbmWorleyDouble(std::string     id,
                  hmap::Vec2<int> shape,
                  hmap::Vec2<int> tiling,
                  float           overlap);

  void compute();

};

class FractalizePath : virtual public ControlNode
{
public:
  FractalizePath(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
};

class GaborNoise : public Primitive
{
public:
  GaborNoise(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  void update_inner_bindings();

  void compute();

};

class Gain : public Filter
{
public:
  Gain(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float gain = 1.f;
};

class GammaCorrection : public Filter
{
public:
  GammaCorrection(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class GammaCorrectionLocal : public Filter
{
public:
  GammaCorrectionLocal(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class GaussianPulse : public Primitive
{
public:
  GaussianPulse(std::string     id,
                hmap::Vec2<int> shape,
                hmap::Vec2<int> tiling,
                float           overlap);

  void compute();

};

class Geomorphons : public Unary
{
public:
  Geomorphons(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class Gradient : virtual public ControlNode
{
public:
  Gradient(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out_dx = hmap::HeightMap();
  hmap::HeightMap value_out_dy = hmap::HeightMap();
};

class GradientAngle : public Unary
{
public:
  GradientAngle(std::string id);

  void compute_in_out(hmap::HeightMap &h, hmap::HeightMap *p_talus);

};

class GradientNorm : public Unary
{
public:
  GradientNorm(std::string id);

  void compute_in_out(hmap::HeightMap &h, hmap::HeightMap *p_talus);

};

class GradientTalus : public Unary
{
public:
  GradientTalus(std::string id);

  void compute_in_out(hmap::HeightMap &h, hmap::HeightMap *p_talus);

};

class HydraulicAlgebric : public Erosion
{
public:
  HydraulicAlgebric(std::string id);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

};

class HydraulicParticle : public Erosion
{
public:
  HydraulicParticle(std::string id);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

};

class HydraulicRidge : public Filter
{
public:
  HydraulicRidge(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class HydraulicStream : public Erosion
{
public:
  HydraulicStream(std::string id);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

};

class HydraulicStreamLog : public Erosion
{
public:
  HydraulicStreamLog(std::string id);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

};

class HydraulicVpipes : public Erosion
{
public:
  HydraulicVpipes(std::string id);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

};

class Import : virtual public ControlNode
{
public:
  Import(std::string     id,
         hmap::Vec2<int> shape,
         hmap::Vec2<int> tiling,
         float           overlap);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class Inverse : public Unary
{
public:
  Inverse(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class Kernel : virtual public ControlNode
{
public:
  Kernel(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::Array value_out = hmap::Array();

  std::map<std::string, int> kernel_map = {
      {"cone", kernel::cone},
      {"cubic_pulse", kernel::cubic_pulse},
      {"lorentzian", kernel::lorentzian},
      {"smooth_cosine", kernel::smooth_cosine}};
};

class KmeansClustering2 : public Binary
{
public:
  KmeansClustering2(std::string id);

  void compute_in_out(hmap::HeightMap &h_out,
                      hmap::HeightMap *p_h_in1,
                      hmap::HeightMap *p_h_in2);

};

class KmeansClustering3 : virtual public ControlNode
{
public:
  KmeansClustering3(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Laplace : public Filter
{
public:
  Laplace(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class LaplaceEdgePreserving : public Filter
{
public:
  LaplaceEdgePreserving(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class Lerp : virtual public ControlNode
{
public:
  Lerp(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class MakeBinary : public Unary
{
public:
  MakeBinary(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class MeanderizePath : virtual public ControlNode
{
public:
  MeanderizePath(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
};

class MeanLocal : public Filter
{
public:
  MeanLocal(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class Median3x3 : public Filter
{
public:
  Median3x3(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class MinimumLocal : public Unary
{
public:
  MinimumLocal(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class MixRGB : virtual public ControlNode
{
public:
  MixRGB(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMapRGB value_out = hmap::HeightMapRGB();
};

class NormalDisplacement : public Filter
{
public:
  NormalDisplacement(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class OneMinus : public Unary
{
public:
  OneMinus(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class Path : virtual public ControlNode
{
public:
  Path(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
};

class PathFinding : virtual public ControlNode
{
public:
  PathFinding(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
};

class PathToHeightmap : virtual public ControlNode
{
public:
  PathToHeightmap(std::string     id,
                  hmap::Vec2<int> shape,
                  hmap::Vec2<int> tiling,
                  float           overlap);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class Peak : public Primitive
{
public:
  Peak(std::string     id,
       hmap::Vec2<int> shape,
       hmap::Vec2<int> tiling,
       float           overlap);

  void compute();

};

class Perlin : public Primitive
{
public:
  Perlin(std::string     id,
         hmap::Vec2<int> shape,
         hmap::Vec2<int> tiling,
         float           overlap);

  void compute();

};

class PerlinBillow : public Primitive
{
public:
  PerlinBillow(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

};

class PingpongPerlin : public Primitive
{
public:
  PingpongPerlin(std::string     id,
                 hmap::Vec2<int> shape,
                 hmap::Vec2<int> tiling,
                 float           overlap);

  void compute();

};

class Plateau : public Filter
{
public:
  Plateau(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class Preview : virtual public ControlNode
{
public:
  Preview(std::string id);

  void compute();

  void update_inner_bindings();

};

class PreviewColorize : virtual public ControlNode
{
public:
  PreviewColorize(std::string id);

  void compute();

  void update_inner_bindings();

};

class RecastCanyon : virtual public ControlNode
{
public:
  RecastCanyon(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class RecastCliff : virtual public ControlNode
{
public:
  RecastCliff(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class RecastCliffDirectional : virtual public ControlNode
{
public:
  RecastCliffDirectional(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class RecastPeak : virtual public ControlNode
{
public:
  RecastPeak(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class RecastRockySlopes : virtual public ControlNode
{
public:
  RecastRockySlopes(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Recurve : public Filter
{
public:
  Recurve(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class RecurveKura : public Filter
{
public:
  RecurveKura(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class RecurveS : public Filter
{
public:
  RecurveS(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class RelativeElevation : public Unary
{
public:
  RelativeElevation(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

 

  int ir = 64;
};

class Remap : public Unary
{
public:
  Remap(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class Rescale : public Unary
{
public:
  Rescale(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class RidgedPerlin : public Primitive
{
public:
  RidgedPerlin(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

};

class Rugosity : public Mask
{
public:
  Rugosity(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class SedimentDeposition : virtual public ControlNode
{
public:
  SedimentDeposition(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class SelectBlobLog : public Mask
{
public:
  SelectBlobLog(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class SelectCavities : public Mask
{
public:
  SelectCavities(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class SelectEq : public Mask
{
public:
  SelectEq(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class SelectElevationSlope : public Mask
{
public:
  SelectElevationSlope(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class SelectGradientNorm : public Mask
{
public:
  SelectGradientNorm(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class SelectInterval : public Mask
{
public:
  SelectInterval(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class SelectPulse : public Mask
{
public:
  SelectPulse(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class SelectRivers : public Mask
{
public:
  SelectRivers(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class SelectTransitions : virtual public ControlNode
{
public:
  SelectTransitions(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Simplex : public Primitive
{
public:
  Simplex(std::string     id,
          hmap::Vec2<int> shape,
          hmap::Vec2<int> tiling,
          float           overlap);

  void compute();

};

class Slope : public Primitive
{
public:
  Slope(std::string     id,
        hmap::Vec2<int> shape,
        hmap::Vec2<int> tiling,
        float           overlap);

  void compute();

  void update_inner_bindings();

};

class SmoothCpulse : public Filter
{
public:
  SmoothCpulse(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class SmoothFill : virtual public ControlNode
{
public:
  SmoothFill(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class SmoothFillHoles : public Filter
{
public:
  SmoothFillHoles(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class SmoothFillSmearPeaks : public Filter
{
public:
  SmoothFillSmearPeaks(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class SteepenConvective : public Filter
{
public:
  SteepenConvective(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class Step : public Primitive
{
public:
  Step(std::string     id,
       hmap::Vec2<int> shape,
       hmap::Vec2<int> tiling,
       float           overlap);

  void compute();

};

class StratifyMultiscale : virtual public ControlNode
{
public:
  StratifyMultiscale(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
};

class StratifyOblique : virtual public ControlNode
{
public:
  StratifyOblique(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Thermal : virtual public ControlNode
{
public:
  Thermal(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class ThermalAutoBedrock : virtual public ControlNode
{
public:
  ThermalAutoBedrock(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class ThermalFlatten : virtual public ControlNode
{
public:
  ThermalFlatten(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class ThermalScree : virtual public ControlNode
{
public:
  ThermalScree(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class ToKernel : virtual public ControlNode
{
public:
  ToKernel(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::Array value_out = hmap::Array();
};

class ToMask : public Mask
{
public:
  ToMask(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class ValleyWidth : public Mask
{
public:
  ValleyWidth(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

};

class ValueNoiseDelaunay : public Primitive
{
public:
  ValueNoiseDelaunay(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap);

  void compute();

};

class ValueNoiseLinear : public Primitive
{
public:
  ValueNoiseLinear(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap);

  void compute();

};

class ValueNoiseThinplate : public Primitive
{
public:
  ValueNoiseThinplate(std::string     id,
                      hmap::Vec2<int> shape,
                      hmap::Vec2<int> tiling,
                      float           overlap);

  void compute();

};

class Warp : virtual public ControlNode
{
public:
  Warp(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class WarpDownslope : public Filter
{
public:
  WarpDownslope(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class WaveDune : public Primitive
{
public:
  WaveDune(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  void compute();

};

class WaveSine : public Primitive
{
public:
  WaveSine(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  void compute();

};

class WaveSquare : public Primitive
{
public:
  WaveSquare(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  void compute();

};

class WaveTriangular : public Primitive
{
public:
  WaveTriangular(std::string     id,
                 hmap::Vec2<int> shape,
                 hmap::Vec2<int> tiling,
                 float           overlap);

  void compute();

};

class White : virtual public ControlNode
{
public:
  White(std::string     id,
        hmap::Vec2<int> shape,
        hmap::Vec2<int> tiling,
        float           overlap);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  int             seed = DEFAULT_SEED;
  float           vmin = 0.f;
  float           vmax = 1.f;

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class WhiteDensityMap : virtual public ControlNode
{
public:
  WhiteDensityMap(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class WhiteSparse : virtual public ControlNode
{
public:
  WhiteSparse(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class Worley : public Primitive
{
public:
  Worley(std::string     id,
         hmap::Vec2<int> shape,
         hmap::Vec2<int> tiling,
         float           overlap);

  void compute();

};

class WorleyDouble : public Primitive
{
public:
  WorleyDouble(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

};

class WorleyValue : public Primitive
{
public:
  WorleyValue(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  void compute();

};

class Wrinkle : public Filter
{
public:
  Wrinkle(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

};

class ZeroedEdges : virtual public ControlNode
{
public:
  ZeroedEdges(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

} // namespace hesiod::cnode
