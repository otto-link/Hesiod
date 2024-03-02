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
#include "hesiod/serialization.hpp"

// clang-format off
#define DEFAULT_KERNEL_SHAPE {17, 17}
#define DEFAULT_KW 2.f
#define DEFAULT_SEED 1

#define CAST_PORT_REF(type, port_id)  static_cast<type *>(this->get_p_data(port_id))
#define CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(enum_value) ControlNodeType get_type() { return ControlNodeType:: enum_value ; } \
                                                            bool serialize_json_v2(std::string fieldName, nlohmann::json& outputData); \
                                                            bool deserialize_json_v2(std::string fieldName, nlohmann::json& inputData);
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

enum class ControlNodeType
{
 INVALID = 0,
 UNARY = 1,
 BINARY = 2,
 DEBUG = 3,
 EROSION = 4,
 FILTER = 5,
 MASK = 6,
 PRIMITIVE = 7,
 CLONE = 8,
 ABS = 9,
 ABS_SMOOTH = 10,
 ALTER_ELEVATION = 11,
 BASE_ELEVATION = 12,
 BEZIER_PATH = 13,
 BIQUAD_PULSE = 14,
 BLEND = 15,
 BUMP = 16,
 BRUSH = 17,
 CALDERA = 18,
 CHECKERBOARD = 19,
 CLAMP = 20,
 CLOUD = 21,
 CLOUD_TO_ARRAY_INTERP = 22,
 COLORIZE = 23,
 COLORIZE_SOLID = 24,
 COMBINE_MASK = 25,
 CONVOLVE_SVD = 26,
 DENDRY = 27,
 DEPRESSION_FILLING = 28,
 DIG_PATH = 29,
 DISTANCE_TRANSFORM = 30,
 EQUALIZE = 31,
 EROSION_MAPS = 32,
 EXPAND_SHRINK = 33,
 EXPAND_SHRINK_DIRECTIONAL = 34,
 EXPORT = 35,
 EXPORT_RGB = 36,
 FACETED = 37,
 FBM_IQ_PERLIN = 38,
 FBM_PERLIN = 39,
 FBM_SIMPLEX = 40,
 FBM_WORLEY = 41,
 FBM_WORLEY_DOUBLE = 42,
 FRACTALIZE_PATH = 43,
 GABOR_NOISE = 44,
 GAIN = 45,
 GAMMA_CORRECTION = 46,
 GAMMA_CORRECTION_LOCAL = 47,
 GAUSSIAN_PULSE = 48,
 GEOMORPHONS = 49,
 GRADIENT = 50,
 GRADIENT_ANGLE = 51,
 GRADIENT_NORM = 52,
 GRADIENT_TALUS = 53,
 HYDRAULIC_ALGEBRIC = 54,
 HYDRAULIC_PARTICLE = 55,
 HYDRAULIC_RIDGE = 56,
 HYDRAULIC_STREAM = 57,
 HYDRAULIC_STREAM_LOG = 58,
 HYDRAULIC_VPIPES = 59,
 IMPORT = 60,
 INVERSE = 61,
 KERNEL = 62,
 KMEANS_CLUSTERING2 = 63,
 KMEANS_CLUSTERING3 = 64,
 LAPLACE = 65,
 LAPLACE_EDGE_PRESERVING = 66,
 LERP = 67,
 MAKE_BINARY = 68,
 MEANDERIZE_PATH = 69,
 MEAN_LOCAL = 70,
 MEDIAN3X3 = 71,
 MINIMUM_LOCAL = 72,
 MIX_RGB = 73,
 NORMAL_DISPLACEMENT = 74,
 ONE_MINUS = 75,
 PATH = 76,
 PATH_FINDING = 77,
 PATH_TO_HEIGHTMAP = 78,
 PEAK = 79,
 PERLIN = 80,
 PERLIN_BILLOW = 81,
 PINGPONG_PERLIN = 82,
 PLATEAU = 83,
 PREVIEW = 84,
 PREVIEW_COLORIZE = 85,
 RECAST_CANYON = 86,
 RECAST_CLIFF = 87,
 RECAST_CLIFF_DIRECTIONAL = 88,
 RECAST_PEAK = 89,
 RECAST_ROCKY_SLOPES = 90,
 RECURVE = 91,
 RECURVE_KURA = 92,
 RECURVE_S = 93,
 RELATIVE_ELEVATION = 94,
 REMAP = 95,
 RESCALE = 96,
 RIDGED_PERLIN = 97,
 RUGOSITY = 98,
 SEDIMENT_DEPOSITION = 99,
 SELECT_BLOB_LOG = 100,
 SELECT_CAVITIES = 101,
 SELECT_EQ = 102,
 SELECT_ELEVATION_SLOPE = 103,
 SELECT_GRADIENT_NORM = 104,
 SELECT_INTERVAL = 105,
 SELECT_PULSE = 106,
 SELECT_RIVERS = 107,
 SELECT_TRANSITIONS = 108,
 SIMPLEX = 109,
 SLOPE = 110,
 SMOOTH_CPULSE = 111,
 SMOOTH_FILL = 112,
 SMOOTH_FILL_HOLES = 113,
 SMOOTH_FILL_SMEAR_PEAKS = 114,
 STEEPEN_CONVECTIVE = 115,
 STEP = 116,
 STRATIFY_MULTISCALE = 117,
 STRATIFY_OBLIQUE = 118,
 THERMAL = 119,
 THERMAL_AUTO_BEDROCK = 120,
 THERMAL_FLATTEN = 121,
 THERMAL_SCREE = 122,
 TO_KERNEL = 123,
 TO_MASK = 124,
 VALLEY_WIDTH = 125,
 VALUE_NOISE_DELAUNAY = 126,
 VALUE_NOISE_LINEAR = 127,
 VALUE_NOISE_THINPLATE = 128,
 WARP = 129,
 WARP_DOWNSLOPE = 130,
 WAVE_DUNE = 131,
 WAVE_SINE = 132,
 WAVE_SQUARE = 133,
 WAVE_TRIANGULAR = 134,
 WHITE = 135,
 WHITE_DENSITY_MAP = 136,
 WHITE_SPARSE = 137,
 WORLEY = 138,
 WORLEY_DOUBLE = 139,
 WORLEY_VALUE = 140,
 WRINKLE = 141,
 ZEROED_EDGES = 142,
};

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

  virtual ControlNodeType get_type() { return ControlNodeType::INVALID; }

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    ar(cereal::make_nvp("id", this->id), cereal::make_nvp("attr", this->attr));
  }
#endif

  SERIALIZATION_V2_IMPLEMENT_BASE();

  void post_process_heightmap(hmap::HeightMap &h);
};

class ControlNodeInstancing
{
public:
  static std::string get_name_from_type(ControlNodeType type);
  static ControlNodeType get_type_from_name(std::string name);
  static std::unique_ptr<ControlNode> create_attribute_from_type(ControlNodeType type, std::string id, void* tree);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(UNARY);

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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(BINARY);

protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Debug : virtual public ControlNode
{
public:
  Debug(std::string id);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(DEBUG);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(EROSION);

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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(FILTER);

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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(MASK);

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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(PRIMITIVE);

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

#ifdef USE_CEREAL
  template <class Archive> void serialize(Archive &ar)
  {
    ControlNode::serialize(ar);

    ar(cereal::make_nvp("id", this->id), cereal::make_nvp("attr", this->attr));

    std::vector<std::string> output_ids = {};

    this->update_inner_bindings();

    for (auto &[port_id, port] : this->get_ports())
      if (port.direction == gnode::direction::out)
        output_ids.push_back(port_id.c_str());

    ar(cereal::make_nvp("output_ids", output_ids));
    ar(cereal::make_nvp("id_count", id_count));

    for (auto &port_id : output_ids)
      if (!this->is_port_id_in_keys(port_id))
        this->add_port(gnode::Port(port_id,
                                   gnode::direction::out,
                                   hesiod::cnode::dtype::dHeightMap));
    this->update_inner_bindings();
  }
#endif

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(CLONE);

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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(ABS);
};

class AbsSmooth : virtual public ControlNode
{
public:
  AbsSmooth(std::string id);

  void update_inner_bindings();

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(ABS_SMOOTH);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class AlterElevation : virtual public ControlNode
{
public:
  AlterElevation(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(ALTER_ELEVATION);

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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(BASE_ELEVATION);
};

class BezierPath : virtual public ControlNode
{
public:
  BezierPath(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(BEZIER_PATH);

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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(BIQUAD_PULSE);
};

class Blend : public Binary
{
public:
  Blend(std::string id);

  void compute_in_out(hmap::HeightMap &h_out,
                      hmap::HeightMap *p_h_in1,
                      hmap::HeightMap *p_h_in2);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(BLEND);

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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(BUMP);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(BRUSH);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(CALDERA);
};

class Checkerboard : public Primitive
{
public:
  Checkerboard(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(CHECKERBOARD);
};

class Clamp : public Unary
{
public:
  Clamp(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(CLAMP);
};

class Cloud : virtual public ControlNode
{
public:
  Cloud(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(CLOUD);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(CLOUD_TO_ARRAY_INTERP);
};

class Colorize : virtual public ControlNode
{
public:
  Colorize(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(COLORIZE);
protected:
  hmap::HeightMapRGB value_out = hmap::HeightMapRGB();
};

class ColorizeSolid : virtual public ControlNode
{
public:
  ColorizeSolid(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(COLORIZE_SOLID);
protected:
  hmap::HeightMapRGB value_out = hmap::HeightMapRGB();
};

class CombineMask : virtual public ControlNode
{
public:
  CombineMask(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(COMBINE_MASK);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(CONVOLVE_SVD);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(DENDRY);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(DEPRESSION_FILLING);
};

class DigPath : virtual public ControlNode
{
public:
  DigPath(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(DIG_PATH);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class DistanceTransform : virtual public ControlNode
{
public:
  DistanceTransform(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(DISTANCE_TRANSFORM);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Equalize : public Filter
{
public:
  Equalize(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(EQUALIZE);
};

class ErosionMaps : virtual public ControlNode
{
public:
  ErosionMaps(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(EROSION_MAPS);
protected:
  hmap::HeightMap erosion_map = hmap::HeightMap();
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class ExpandShrink : public Filter
{
public:
  ExpandShrink(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(EXPAND_SHRINK);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(EXPAND_SHRINK_DIRECTIONAL);
};

class Export : virtual public ControlNode
{
public:
  Export(std::string id);

  void compute();

  void write_file();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(EXPORT);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(EXPORT_RGB);
};

class Faceted : virtual public ControlNode
{
public:
  Faceted(std::string id);

  void update_inner_bindings();

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(FACETED);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(FBM_IQ_PERLIN);
};

class FbmPerlin : public Primitive
{
public:
  FbmPerlin(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(FBM_PERLIN);
};

class FbmSimplex : public Primitive
{
public:
  FbmSimplex(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(FBM_SIMPLEX);
};

class FbmWorley : public Primitive
{
public:
  FbmWorley(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(FBM_WORLEY);
};

class FbmWorleyDouble : public Primitive
{
public:
  FbmWorleyDouble(std::string     id,
                  hmap::Vec2<int> shape,
                  hmap::Vec2<int> tiling,
                  float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(FBM_WORLEY_DOUBLE);
};

class FractalizePath : virtual public ControlNode
{
public:
  FractalizePath(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(FRACTALIZE_PATH);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(GABOR_NOISE);
};

class Gain : public Filter
{
public:
  Gain(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(GAIN);
protected:
  float gain = 1.f;
};

class GammaCorrection : public Filter
{
public:
  GammaCorrection(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(GAMMA_CORRECTION);
};

class GammaCorrectionLocal : public Filter
{
public:
  GammaCorrectionLocal(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(GAMMA_CORRECTION_LOCAL);
};

class GaussianPulse : public Primitive
{
public:
  GaussianPulse(std::string     id,
                hmap::Vec2<int> shape,
                hmap::Vec2<int> tiling,
                float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(GAUSSIAN_PULSE);
};

class Geomorphons : public Unary
{
public:
  Geomorphons(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(GEOMORPHONS);
};

class Gradient : virtual public ControlNode
{
public:
  Gradient(std::string id);

  void update_inner_bindings();

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(GRADIENT);
protected:
  hmap::HeightMap value_out_dx = hmap::HeightMap();
  hmap::HeightMap value_out_dy = hmap::HeightMap();
};

class GradientAngle : public Unary
{
public:
  GradientAngle(std::string id);

  void compute_in_out(hmap::HeightMap &h, hmap::HeightMap *p_talus);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(GRADIENT_ANGLE);
};

class GradientNorm : public Unary
{
public:
  GradientNorm(std::string id);

  void compute_in_out(hmap::HeightMap &h, hmap::HeightMap *p_talus);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(GRADIENT_NORM);
};

class GradientTalus : public Unary
{
public:
  GradientTalus(std::string id);

  void compute_in_out(hmap::HeightMap &h, hmap::HeightMap *p_talus);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(GRADIENT_TALUS);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(HYDRAULIC_ALGEBRIC);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(HYDRAULIC_PARTICLE);
};

class HydraulicRidge : public Filter
{
public:
  HydraulicRidge(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(HYDRAULIC_RIDGE);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(HYDRAULIC_STREAM);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(HYDRAULIC_STREAM_LOG);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(HYDRAULIC_VPIPES);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(IMPORT);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(INVERSE);
};

class Kernel : virtual public ControlNode
{
public:
  Kernel(std::string id);

  void update_inner_bindings();

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(KERNEL);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(KMEANS_CLUSTERING2);
};

class KmeansClustering3 : virtual public ControlNode
{
public:
  KmeansClustering3(std::string id);

  void update_inner_bindings();

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(KMEANS_CLUSTERING3);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Laplace : public Filter
{
public:
  Laplace(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(LAPLACE);
};

class LaplaceEdgePreserving : public Filter
{
public:
  LaplaceEdgePreserving(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(LAPLACE_EDGE_PRESERVING);
};

class Lerp : virtual public ControlNode
{
public:
  Lerp(std::string id);

  void update_inner_bindings();

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(LERP);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class MakeBinary : public Unary
{
public:
  MakeBinary(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(MAKE_BINARY);
};

class MeanderizePath : virtual public ControlNode
{
public:
  MeanderizePath(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(MEANDERIZE_PATH);
protected:
  hmap::Path value_out = hmap::Path();
};

class MeanLocal : public Filter
{
public:
  MeanLocal(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(MEAN_LOCAL);
};

class Median3x3 : public Filter
{
public:
  Median3x3(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(MEDIAN3X3);
};

class MinimumLocal : public Unary
{
public:
  MinimumLocal(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(MINIMUM_LOCAL);
};

class MixRGB : virtual public ControlNode
{
public:
  MixRGB(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(MIX_RGB);
protected:
  hmap::HeightMapRGB value_out = hmap::HeightMapRGB();
};

class NormalDisplacement : public Filter
{
public:
  NormalDisplacement(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(NORMAL_DISPLACEMENT);
};

class OneMinus : public Unary
{
public:
  OneMinus(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(ONE_MINUS);
};

class Path : virtual public ControlNode
{
public:
  Path(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(PATH);
protected:
  hmap::Path value_out = hmap::Path();
};

class PathFinding : virtual public ControlNode
{
public:
  PathFinding(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(PATH_FINDING);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(PATH_TO_HEIGHTMAP);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(PEAK);
};

class Perlin : public Primitive
{
public:
  Perlin(std::string     id,
         hmap::Vec2<int> shape,
         hmap::Vec2<int> tiling,
         float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(PERLIN);
};

class PerlinBillow : public Primitive
{
public:
  PerlinBillow(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(PERLIN_BILLOW);
};

class PingpongPerlin : public Primitive
{
public:
  PingpongPerlin(std::string     id,
                 hmap::Vec2<int> shape,
                 hmap::Vec2<int> tiling,
                 float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(PINGPONG_PERLIN);
};

class Plateau : public Filter
{
public:
  Plateau(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(PLATEAU);
};

class Preview : virtual public ControlNode
{
public:
  Preview(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(PREVIEW);
};

class PreviewColorize : virtual public ControlNode
{
public:
  PreviewColorize(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(PREVIEW_COLORIZE);
};

class RecastCanyon : virtual public ControlNode
{
public:
  RecastCanyon(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RECAST_CANYON);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class RecastCliff : virtual public ControlNode
{
public:
  RecastCliff(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RECAST_CLIFF);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class RecastCliffDirectional : virtual public ControlNode
{
public:
  RecastCliffDirectional(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RECAST_CLIFF_DIRECTIONAL);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class RecastPeak : virtual public ControlNode
{
public:
  RecastPeak(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RECAST_PEAK);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class RecastRockySlopes : virtual public ControlNode
{
public:
  RecastRockySlopes(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RECAST_ROCKY_SLOPES);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Recurve : public Filter
{
public:
  Recurve(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);
  
  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RECURVE);
};

class RecurveKura : public Filter
{
public:
  RecurveKura(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RECURVE_KURA);
};

class RecurveS : public Filter
{
public:
  RecurveS(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RECURVE_S);
};

class RelativeElevation : public Unary
{
public:
  RelativeElevation(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RELATIVE_ELEVATION);

  int  ir = 64;
};

class Remap : public Unary
{
public:
  Remap(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(REMAP);
};

class Rescale : public Unary
{
public:
  Rescale(std::string id);

  void compute_in_out(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RESCALE);
};

class RidgedPerlin : public Primitive
{
public:
  RidgedPerlin(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RIDGED_PERLIN);
};

class Rugosity : public Mask
{
public:
  Rugosity(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(RUGOSITY);
};

class SedimentDeposition : virtual public ControlNode
{
public:
  SedimentDeposition(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SEDIMENT_DEPOSITION);
protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class SelectBlobLog : public Mask
{
public:
  SelectBlobLog(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SELECT_BLOB_LOG);
};

class SelectCavities : public Mask
{
public:
  SelectCavities(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SELECT_CAVITIES);
};

class SelectEq : public Mask
{
public:
  SelectEq(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SELECT_EQ);
};

class SelectElevationSlope : public Mask
{
public:
  SelectElevationSlope(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SELECT_ELEVATION_SLOPE);
};

class SelectGradientNorm : public Mask
{
public:
  SelectGradientNorm(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SELECT_GRADIENT_NORM);
};

class SelectInterval : public Mask
{
public:
  SelectInterval(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SELECT_INTERVAL);
};

class SelectPulse : public Mask
{
public:
  SelectPulse(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SELECT_PULSE);
};

class SelectRivers : public Mask
{
public:
  SelectRivers(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SELECT_RIVERS);
};

class SelectTransitions : virtual public ControlNode
{
public:
  SelectTransitions(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SELECT_TRANSITIONS);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SIMPLEX);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SLOPE);
};

class SmoothCpulse : public Filter
{
public:
  SmoothCpulse(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SMOOTH_CPULSE);
};

class SmoothFill : virtual public ControlNode
{
public:
  SmoothFill(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SMOOTH_FILL);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
  hmap::HeightMap deposition_map = hmap::HeightMap();
};

class SmoothFillHoles : public Filter
{
public:
  SmoothFillHoles(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SMOOTH_FILL_HOLES);
};

class SmoothFillSmearPeaks : public Filter
{
public:
  SmoothFillSmearPeaks(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(SMOOTH_FILL_SMEAR_PEAKS);
};

class SteepenConvective : public Filter
{
public:
  SteepenConvective(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(STEEPEN_CONVECTIVE);
};

class Step : public Primitive
{
public:
  Step(std::string     id,
       hmap::Vec2<int> shape,
       hmap::Vec2<int> tiling,
       float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(STEP);
};

class StratifyMultiscale : virtual public ControlNode
{
public:
  StratifyMultiscale(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(STRATIFY_MULTISCALE);
protected:
  hmap::HeightMap value_out = hmap::HeightMap(); // eroded heightmap
};

class StratifyOblique : virtual public ControlNode
{
public:
  StratifyOblique(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(STRATIFY_OBLIQUE);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class Thermal : virtual public ControlNode
{
public:
  Thermal(std::string id);

  void compute();

  void update_inner_bindings();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(THERMAL);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(THERMAL_AUTO_BEDROCK);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(THERMAL_FLATTEN);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(THERMAL_SCREE);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(TO_KERNEL);
protected:
  hmap::Array value_out = hmap::Array();
};

class ToMask : public Mask
{
public:
  ToMask(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(TO_MASK);
};

class ValleyWidth : public Mask
{
public:
  ValleyWidth(std::string id);

  void compute_mask(hmap::HeightMap &h_out, hmap::HeightMap *p_h_in);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(VALLEY_WIDTH);
};

class ValueNoiseDelaunay : public Primitive
{
public:
  ValueNoiseDelaunay(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(VALUE_NOISE_DELAUNAY);
};

class ValueNoiseLinear : public Primitive
{
public:
  ValueNoiseLinear(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(VALUE_NOISE_LINEAR);
};

class ValueNoiseThinplate : public Primitive
{
public:
  ValueNoiseThinplate(std::string     id,
                      hmap::Vec2<int> shape,
                      hmap::Vec2<int> tiling,
                      float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(VALUE_NOISE_THINPLATE);
};

class Warp : virtual public ControlNode
{
public:
  Warp(std::string id);

  void update_inner_bindings();

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WARP);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

class WarpDownslope : public Filter
{
public:
  WarpDownslope(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WARP_DOWNSLOPE);
};

class WaveDune : public Primitive
{
public:
  WaveDune(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WAVE_DUNE);
};

class WaveSine : public Primitive
{
public:
  WaveSine(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WAVE_SINE);
};

class WaveSquare : public Primitive
{
public:
  WaveSquare(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WAVE_SQUARE);
};

class WaveTriangular : public Primitive
{
public:
  WaveTriangular(std::string     id,
                 hmap::Vec2<int> shape,
                 hmap::Vec2<int> tiling,
                 float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WAVE_TRIANGULAR);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WHITE);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WHITE_DENSITY_MAP);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WHITE_SPARSE);
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

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WORLEY);
};

class WorleyDouble : public Primitive
{
public:
  WorleyDouble(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WORLEY_DOUBLE);
};

class WorleyValue : public Primitive
{
public:
  WorleyValue(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  void compute();
  
  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WORLEY_VALUE);
};

class Wrinkle : public Filter
{
public:
  Wrinkle(std::string id);

  void compute_filter(hmap::HeightMap &h, hmap::HeightMap *p_mask);

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(WRINKLE);
};

class ZeroedEdges : virtual public ControlNode
{
public:
  ZeroedEdges(std::string id);

  void update_inner_bindings();

  void compute();

  CONTROL_NODE_IMPLEMENT_SERIALIZATION_V2(ZEROED_EDGES);
protected:
  hmap::HeightMap value_out = hmap::HeightMap();
};

} // namespace hesiod::cnode
