/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "gnode.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include "hesiod/attribute.hpp"

// clang-format off
#define DEFAULT_KERNEL_SHAPE {17, 17}
#define DEFAULT_KW 2.f
#define DEFAULT_SEED 1
// clang-format on

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
    {"CombineMask", "Mask"},
    {"PreviewColorize", "Texture"},
    {"ConvolveSVD", "Math/Convolution"},
    {"Debug", "Debug"},
    {"DepressionFilling", "Erosion"}, // not distributed
    {"DigPath", "Roads"},             // partially distributed
    {"DistanceTransform", "Math"},
    {"Equalize", "Filter/Recurve"}, // not distributed
    {"ErosionMaps", "Erosion/Hydraulic"},
    {"ExpandShrink", "Filter/Recast"},
    {"ExpandShrinkDirectional", "Filter/Recast"},
    {"Export", "IO/Files"},
    {"ExportRGB", "IO/Files"},
    {"FbmPerlin", "Primitive/Coherent Noise"},
    {"FbmSimplex", "Primitive/Coherent Noise"},
    {"FbmWorley", "Primitive/Coherent Noise"},
    {"FractalizePath", "Geometry/Path"},
    {"GaborNoise", "Primitive/Coherent Noise"},
    {"Gain", "Filter/Recurve"},
    {"GammaCorrection", "Filter/Recurve"},
    {"GammaCorrectionLocal", "Filter/Recurve"},
    {"GaussianPulse", "Primitive/Function"},
    {"Gradient", "Math/Gradient"},
    {"GradientAngle", "Math/Gradient"},
    {"GradientNorm", "Math/Gradient"},
    {"GradientTalus", "Math/Gradient"},
    {"HydraulicAlgebric", "Erosion/Hydraulic"},
    // {"HydraulicBenes", "Erosion/Hydraulic"}, // BROKEN
    {"HydraulicParticle", "Erosion/Hydraulic"},
    {"HydraulicRidge", "Erosion/Hydraulic"}, // not distributed
    {"HydraulicStream", "Erosion/Hydraulic"},
    {"HydraulicStreamLog", "Erosion/Hydraulic"},
    {"HydraulicVpipes", "Erosion/Hydraulic"},
    {"Import", "IO/Files"},
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
    {"Perlin", "Primitive/Coherent Noise"},
    {"PerlinBillow", "Primitive/Coherent Noise"},
    {"Plateau", "Filter/Recurve"},
    {"Preview", "Debug"},
    {"RecastCanyon", "Filter/Recast"},
    {"Recurve", "Filter/Recurve"},
    {"RecurveKura", "Filter/Recurve"},
    {"RecurveS", "Filter/Recurve"},
    {"RelativeElevation", "Features"},
    {"Remap", "Filter/Range"},
    {"RidgedPerlin", "Primitive/Coherent Noise"},
    {"Rugosity", "Features"},
    {"SedimentDeposition", "Erosion/Thermal"},
    {"SelectCavities", "Mask"},
    {"SelectEq", "Mask"},
    {"SelectGradientNorm", "Mask"},
    {"SelectInterval", "Mask"},
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
    {"White", "Primitive/Random"},
    {"WhiteDensityMap", "Primitive/Random"},
    {"WhiteSparse", "Primitive/Random"},
    {"Worley", "Primitive/Coherent Noise"},
    {"WorleyDouble", "Primitive/Coherent Noise"},
    {"WorleyValue", "Primitive/Coherent Noise"},
    {"ZeroedEdges", "Math/Boundaries"}};

//----------------------------------------
// Base node class
//----------------------------------------

class ControlNode : public gnode::Node
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

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    ar(cereal::make_nvp("id", this->id), cereal::make_nvp("attr", this->attr));
  }
#endif

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

// OK
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

// OK
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

  void remove_unused_outputs();

  void update_inner_bindings();

  void compute();

protected:
  int             id_count = 0;
  int             n_outputs = 1;
  hmap::HeightMap value_out = hmap::HeightMap();
};

//----------------------------------------
// End-user nodes
//----------------------------------------

class AlterElevation : virtual public ControlNode
{
public:
  AlterElevation(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  int             ir = 64;
  float           footprint_ratio = 1.f;
  float           vmin = 0.f;
  float           vmax = 1.f;
};

// OK
class BaseElevation : public Primitive
{
public:
  BaseElevation(std::string     id,
                hmap::Vec2<int> shape,
                hmap::Vec2<int> tiling,
                float           overlap);

  void compute();
};

// OK
class BezierPath : virtual public ControlNode
{
public:
  BezierPath(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
};

// OK
class BiquadPulse : public Primitive
{
public:
  BiquadPulse(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  void compute();
};

// OK
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

// OK
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

// OK
class Caldera : public Primitive
{
public:
  Caldera(std::string     id,
          hmap::Vec2<int> shape,
          hmap::Vec2<int> tiling,
          float           overlap);

  void compute();
};

// OK
class Checkerboard : public Primitive
{
public:
  Checkerboard(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();
};

// OK
class Clamp : public Unary
{
public:
  Clamp(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);
};

// TO DO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class Cloud : virtual public ControlNode
{
public:
  Cloud(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Cloud value_out = hmap::Cloud();
};

// OK
class CloudToArrayInterp : public Primitive
{
public:
  CloudToArrayInterp(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap);

  void compute();
};

// OK
class Colorize : virtual public ControlNode
{
public:
  Colorize(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMapRGB value_out = hmap::HeightMapRGB();
};

// OK
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

// OK
class ConvolveSVD : virtual public ControlNode
{
public:
  ConvolveSVD(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

// OK
class DepressionFilling : public Unary
{
public:
  DepressionFilling(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);
};

// OK
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
  hmap::Vec2<int> shape_working = {512, 512};
  bool            reverse = true;
  float           vmin = 0.f;
  float           vmax = 1.f;
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
  float           tolerance = 0.f;
};

class ExpandShrink : public Filter
{
public:
  ExpandShrink(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int                        ir = 4;
  bool                       shrink = false;
  std::map<std::string, int> kernel_map = {
      {"cone", kernel::cone},
      {"cubic_pulse", kernel::cubic_pulse},
      {"lorentzian", kernel::lorentzian},
      {"smooth_cosine", kernel::smooth_cosine}};
  int kernel = kernel::cubic_pulse;
};

class ExpandShrinkDirectional : public Filter
{
public:
  ExpandShrinkDirectional(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int   ir = 4;
  float angle = 30.f;
  float aspect_ratio = 0.2f;
  float anisotropy = 1.f;
  bool  shrink = false;
};

class Export : virtual public ControlNode
{
public:
  Export(std::string id);

  void compute();

  void write_file();

protected:
  bool        auto_export = false;
  int         export_format = export_type::png8bit;
  std::string fname = "export.png";
};

class ExportRGB : virtual public ControlNode
{
public:
  ExportRGB(std::string id);

  void compute();

  void write_file();

protected:
  bool        auto_export = false;
  std::string fname = "export_rgb.png";
};

// OK
class FbmPerlin : public Primitive
{
public:
  FbmPerlin(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  void compute();
};

// OK
class FbmSimplex : public Primitive
{
public:
  FbmSimplex(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  void compute();
};

// OK
class FbmWorley : public Primitive
{
public:
  FbmWorley(std::string     id,
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
  int        iterations = 1;
  int        seed = DEFAULT_SEED;
  float      sigma = 0.3f;
  int        orientation = 0;
  float      persistence = 1.f;
};

class GaborNoise : virtual public ControlNode
{
public:
  GaborNoise(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           kw = 1.f;
  float           angle = 30.f;
  int             width = 128;
  float           density = 0.05f;
  int             seed = DEFAULT_SEED;
  float           vmin = 0.f;
  float           vmax = 1.f;

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

// OK
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

protected:
  float gamma = 1.f;
};

class GammaCorrectionLocal : public Filter
{
public:
  GammaCorrectionLocal(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float gamma = 1.f;
  int   ir = 4;
  float k = 0.1f;
};

// OK
class GaussianPulse : public Primitive
{
public:
  GaussianPulse(std::string     id,
                hmap::Vec2<int> shape,
                hmap::Vec2<int> tiling,
                float           overlap);

  void compute();
};

// OK
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

// OK
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

protected:
  float talus_global = 2.f;
  int   ir = 16;
  float c_erosion = 0.07f;
  float c_deposition = 0.01f;
  int   iterations = 1;
};

class HydraulicBenes : public Erosion
{
public:
  HydraulicBenes(std::string id);

  void compute_erosion(hmap::HeightMap &h,
                       hmap::HeightMap *p_bedrock,
                       hmap::HeightMap *p_moisture_map,
                       hmap::HeightMap *p_mask,
                       hmap::HeightMap *p_erosion_map,
                       hmap::HeightMap *p_deposition_map);

protected:
  int   iterations = 50;
  float c_capacity = 40.f;
  float c_erosion = 0.2f;
  float c_deposition = 0.8f;
  float water_level = 0.005f;
  float evap_rate = 0.01f;
  float rain_rate = 0.5f;
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

protected:
  int   seed = 1;
  int   nparticles = 40000;
  int   c_radius = 0;
  float c_capacity = 40.f;
  float c_erosion = 0.05f;
  float c_deposition = 0.1f;
  float drag_rate = 0.01f;
  float evap_rate = 0.001f;
};

class HydraulicRidge : public Filter
{
public:
  HydraulicRidge(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float talus_global = 16.f;
  float intensity = 0.5f;
  float erosion_factor = 1.5f;
  float smoothing_factor = 0.5f;
  float noise_ratio = 0.1f;
  int   ir = 16;
  int   seed = DEFAULT_SEED;
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

protected:
  float c_erosion = 0.05f;
  float talus_ref = 0.1f;
  int   ir = 1;
  float clipping_ratio = 10.f;
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

protected:
  float c_erosion = 0.05f;
  float talus_ref = 0.1f;
  float gamma = 1.f;
  int   ir = 1;
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

protected:
  int   iterations = 50;
  float water_height = 0.01f;
  float c_capacity = 0.1f;
  float c_erosion = 0.01f;
  float c_deposition = 0.01f;
  float rain_rate = 0.f;
  float evap_rate = 0.01f;
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
  float           vmin = 0.f;
  float           vmax = 1.f;
  std::string     fname = "";

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class Kernel : virtual public ControlNode
{
public:
  Kernel(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::Array     value_out = hmap::Array();
  bool            normalized = true;
  float           vmin = 0.f;
  float           vmax = 1.f;
  hmap::Vec2<int> shape = DEFAULT_KERNEL_SHAPE;
  int             kernel = kernel::cubic_pulse;

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

protected:
  int               nclusters = 4;
  hmap::Vec2<float> weights = {1.f, 1.f};
  int               seed = DEFAULT_SEED;
  hmap::Vec2<int>   shape_clustering = {256, 256};
  bool              normalize_inputs = true;
};

class KmeansClustering3 : virtual public ControlNode
{
public:
  KmeansClustering3(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap   value_out = hmap::HeightMap();
  int               nclusters = 4;
  hmap::Vec3<float> weights = {1.f, 1.f, 1.f};
  int               seed = DEFAULT_SEED;
  hmap::Vec2<int>   shape_clustering = {256, 256};
  bool              normalize_inputs = true;
};

class Laplace : public Filter
{
public:
  Laplace(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float sigma = 0.2f;
  int   iterations = 3;
};

class LaplaceEdgePreserving : public Filter
{
public:
  LaplaceEdgePreserving(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float sigma = 0.2f;
  int   iterations = 3;
  float talus_global = 10.f;
};

class Lerp : virtual public ControlNode
{
public:
  Lerp(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  float           t = 0.5f;
};

class MakeBinary : public Unary
{
public:
  MakeBinary(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  float threshold = 0.f;
};

class MeanderizePath : virtual public ControlNode
{
public:
  MeanderizePath(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
  float      radius = 0.05f;
  float      tangent_contribution = 0.1f;
  int        iterations = 1;
  float      transition_length_ratio = 0.2f;
};

class MeanLocal : public Filter
{
public:
  MeanLocal(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int ir = 8;
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

protected:
  int ir = 4;
};

class MixRGB : virtual public ControlNode
{
public:
  MixRGB(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMapRGB value_out = hmap::HeightMapRGB();
  float              t = 0.5f;
  bool               sqrt_mix = true;
};

class NormalDisplacement : public Filter
{
public:
  NormalDisplacement(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float amount = 5.f;
  int   ir = 0;
  bool  reverse = false;
};

class OneMinus : public Unary
{
public:
  OneMinus(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  float vmin = 0.f;
  float vmax = 1.f;
};

class Path : virtual public ControlNode
{
public:
  Path(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path value_out = hmap::Path();
  bool       closed = false;
};

class PathFinding : virtual public ControlNode
{
public:
  PathFinding(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::Path      value_out = hmap::Path();
  hmap::Vec2<int> wshape = {256, 256};
  float           elevation_ratio = 0.5f;
  float           distance_exponent = 1.f;
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
  bool            filled = false;
  float           vmin = 0.f;
  float           vmax = 1.f;

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

// OK
class Perlin : public Primitive
{
public:
  Perlin(std::string     id,
         hmap::Vec2<int> shape,
         hmap::Vec2<int> tiling,
         float           overlap);

  void compute();
};

// OK
class PerlinBillow : public Primitive
{
public:
  PerlinBillow(std::string     id,
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

protected:
  int   ir = 32;
  float factor = 4.f;
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
  float           vcut = 0.7f;
  float           gamma = 4.f;
};

class Recurve : public Filter
{
public:
  Recurve(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  std::vector<float> curve = {0.f, 0.25f, 0.5f, 0.75f, 1.f};
};

// OK
class RecurveKura : public Filter
{
public:
  RecurveKura(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);
};

// OK
class RecurveS : public Filter
{
public:
  RecurveS(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);
};

// OK
class RelativeElevation : public Unary
{
public:
  RelativeElevation(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);
  int  ir = 64;
};

// OK
class Remap : public Unary
{
public:
  Remap(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);
};

// OK
class RidgedPerlin : public Primitive
{
public:
  RidgedPerlin(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();
};

// OK
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
  float           talus_global = 0.1f;
  float           max_deposition = 0.01;
  int             iterations = 5;
  int             thermal_subiterations = 10;
};

// OK
class SelectCavities : public Mask
{
public:
  SelectCavities(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);
};

// TO DO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class SelectEq : public Mask
{
public:
  SelectEq(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

protected:
  float value = 0.f;
};

// OK
class SelectGradientNorm : public Mask
{
public:
  SelectGradientNorm(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);
};

// OK
class SelectInterval : public Mask
{
public:
  SelectInterval(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);
};

// OK
class SelectRivers : public Mask
{
public:
  SelectRivers(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);
};

// OK
class SelectTransitions : virtual public ControlNode
{
public:
  SelectTransitions(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

// OK
class Simplex : public Primitive
{
public:
  Simplex(std::string     id,
          hmap::Vec2<int> shape,
          hmap::Vec2<int> tiling,
          float           overlap);

  void compute();
};

class Slope : virtual public ControlNode
{
public:
  Slope(std::string     id,
        hmap::Vec2<int> shape,
        hmap::Vec2<int> tiling,
        float           overlap);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap   value_out = hmap::HeightMap();
  float             angle = 0.f;
  float             talus_global = 4.f;
  hmap::Vec2<float> center = {0.5f, 0.5f};

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;
};

class SmoothCpulse : public Filter
{
public:
  SmoothCpulse(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int ir = 8;
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
  int             ir = 32;
  float           k = 0.01f;
};

class SmoothFillHoles : public Filter
{
public:
  SmoothFillHoles(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int ir = 8;
};

class SmoothFillSmearPeaks : public Filter
{
public:
  SmoothFillSmearPeaks(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  int ir = 8;
};

class SteepenConvective : public Filter
{
public:
  SteepenConvective(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float angle = 0.f;
  int   iterations = 1;
  int   ir = 0;
  float dt = 0.1f;
};

// OK
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
  hmap::HeightMap    value_out = hmap::HeightMap(); // eroded heightmap
  std::vector<int>   n_strata = {2, 3, 4};
  std::vector<float> strata_noise = {0.f, 0.f, 0.f};
  std::vector<float> gamma_list = {1.5f, 0.5f, 1.f};
  std::vector<float> gamma_noise = {0.f, 0.f, 0.f};
  int                seed = DEFAULT_SEED;
};

class StratifyOblique : virtual public ControlNode
{
public:
  StratifyOblique(std::string id);

  void compute();

  void update_inner_bindings();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  int             n_strata = 3;
  float           strata_noise = 0.f;
  float           gamma = 0.7f;
  float           gamma_noise = 0.f;
  float           talus_global = 2.f;
  float           angle = 30.f;
  int             seed = DEFAULT_SEED;
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
  float           talus_global = 0.1f;
  int             iterations = 10;
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
  float           talus_global = 0.1f;
  int             iterations = 10;
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
  float           talus_global = 3.f;
  int             seed = DEFAULT_SEED;
  float           zmax = 0.3f;
  float           zmin = -1.f;
  float           noise_ratio = 0.3f;
  float           landing_talus_ratio = 1.f;
  float           landing_width_ratio = 0.25f;
  bool            talus_constraint = true;
};

// OK
class ToMask : public Mask
{
public:
  ToMask(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);
};

// OK
class ValleyWidth : public Mask
{
public:
  ValleyWidth(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);
};

// OK
class ValueNoiseDelaunay : public Primitive
{
public:
  ValueNoiseDelaunay(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap);

  void compute();
};

// OK
class ValueNoiseLinear : public Primitive
{
public:
  ValueNoiseLinear(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap);

  void compute();
};

// OK
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
  float           scale = 1.f;
};

class WarpDownslope : public Filter
{
public:
  WarpDownslope(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

protected:
  float amount = 5.f;
  int   ir = 0;
  bool  reverse = false;
};

// OK
class WaveDune : public Primitive
{
public:
  WaveDune(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  void compute();
};

// OK
class WaveSine : public Primitive
{
public:
  WaveSine(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  void compute();
};

// OK
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

// OK
class WhiteDensityMap : virtual public ControlNode
{
public:
  WhiteDensityMap(std::string id);

  void update_inner_bindings();

  void compute();

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

// OK
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

// OK
class Worley : public Primitive
{
public:
  Worley(std::string     id,
         hmap::Vec2<int> shape,
         hmap::Vec2<int> tiling,
         float           overlap);

  void compute();
};

// OK
class WorleyDouble : public Primitive
{
public:
  WorleyDouble(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();
};

// OK
class WorleyValue : public Primitive
{
public:
  WorleyValue(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  void compute();
};

// OK
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
