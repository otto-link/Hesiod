/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "gnode.hpp"
#include <GLFW/glfw3.h>
#include <cereal/archives/json.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "hesiod/control_node.hpp"

namespace hesiod::vnode
{

enum preview_type : int
{
  grayscale,
  histogram
};

struct viewnode_color_set
{
  uint32_t base;
  uint32_t hovered;

  viewnode_color_set(hmap::Vec4<int> rgba)
  {
    this->base = IM_COL32(rgba.a, rgba.b, rgba.c, (int)(0.3f * rgba.d));
    this->hovered = IM_COL32(rgba.a, rgba.b, rgba.c, rgba.d);
  }
};

/* // Nord theme
static const std::map<std::string, viewnode_color_set> category_colors = {
    {"Debug", viewnode_color_set({200, 0, 0, 255})},
    {"Math", viewnode_color_set({46, 52, 64, 255})},
    {"Geometry", viewnode_color_set({67, 76, 94, 255})},
    {"Roads", viewnode_color_set({76, 86, 106, 255})},
    {"Routing", viewnode_color_set({216, 222, 233, 255})},
    {"IO", viewnode_color_set({229, 233, 240, 255})},
    {"Primitive", viewnode_color_set({143, 188, 187, 255})},
    {"Hydrology", viewnode_color_set({136, 192, 208, 255})},
    {"Operator", viewnode_color_set({129, 161, 193, 255})},
    {"Filter", viewnode_color_set({94, 129, 172, 255})},
    {"Features", viewnode_color_set({191, 97, 106, 255})},
    {"Erosion", viewnode_color_set({235, 203, 139, 255})},
    {"Biomes", viewnode_color_set({163, 190, 140, 255})},
    {"Mask", viewnode_color_set({180, 142, 173, 255})}}; */

/* // OceanicNext theme
static const std::map<std::string, viewnode_color_set> category_colors = {
    {"Debug", viewnode_color_set({200, 0, 0, 255})},
    {"Math", viewnode_color_set({27, 43, 52, 255})},
    {"Geometry", viewnode_color_set({52, 61, 70, 255})},
    {"Roads", viewnode_color_set({79, 91, 102, 255})},
    {"Filter", viewnode_color_set({101, 115, 126, 255})},
    {"Operator", viewnode_color_set({167, 173, 186, 255})},
    {"Routing", viewnode_color_set({192, 197, 206, 255})},
    {"IO", viewnode_color_set({205, 211, 222, 255})},
    {"Features", viewnode_color_set({249, 145, 87, 255})},
    {"Erosion", viewnode_color_set({250, 200, 99, 255})},
    {"Biomes", viewnode_color_set({153, 199, 148, 255})},
    {"Primitive", viewnode_color_set({98, 179, 178, 255})},
    {"Hydrology", viewnode_color_set({102, 153, 204, 255})},
    {"Mask", viewnode_color_set({197, 148, 197, 255})}}; */

// Solarized
static const std::map<std::string, viewnode_color_set> category_colors = {
    {"Debug", viewnode_color_set({200, 0, 0, 255})},
    {"Math", viewnode_color_set({0, 43, 54, 255})},
    {"Geometry", viewnode_color_set({101, 123, 131, 255})},
    {"Roads", viewnode_color_set({147, 161, 161, 255})},
    {"Routing", viewnode_color_set({188, 182, 163, 255})},
    {"IO", viewnode_color_set({203, 196, 177, 255})},
    {"Features", viewnode_color_set({181, 137, 0, 255})},
    {"Erosion", viewnode_color_set({203, 75, 22, 255})},
    {"Mask", viewnode_color_set({211, 54, 130, 255})},
    {"Filter", viewnode_color_set({108, 113, 196, 255})},
    {"Operator", viewnode_color_set({108, 113, 196, 255})},
    {"Hydrology", viewnode_color_set({38, 139, 210, 255})},
    {"Primitive", viewnode_color_set({42, 161, 152, 255})},
    {"Biomes", viewnode_color_set({133, 153, 0, 255})}};

// Dracula theme for links
static const std::map<int, viewnode_color_set> dtype_colors = {
    {hesiod::cnode::dArray, viewnode_color_set({255, 121, 198, 255})},
    {hesiod::cnode::dCloud, viewnode_color_set({139, 233, 253, 255})},
    {hesiod::cnode::dHeightMap, viewnode_color_set({255, 255, 255, 255})},
    {hesiod::cnode::dPath, viewnode_color_set({80, 250, 123, 255})}};

class ViewNode
{
public:
  ViewNode();

  ViewNode(gnode::Node *p_control_node);

  gnode::Node *get_p_control_node();

  std::string get_preview_port_id();

  void set_p_control_node(gnode::Node *new_p_control_node);

  void set_preview_port_id(std::string new_port_id);

  void set_preview_type(int new_preview_type);

  virtual void post_control_node_update();

  virtual bool render_settings();

  void render_node();

  virtual void render_node_specific_content(){};

  bool render_settings_header();

  bool render_settings_footer();

  virtual void serialize_load(cereal::JSONInputArchive &);
  virtual void serialize_save(cereal::JSONOutputArchive &);

  bool trigger_update_after_edit();

  void update_preview();

protected:
  std::string preview_port_id = "";
  bool        show_preview = true;
  float       node_width = 128.f;

private:
  gnode::Node    *p_control_node;
  hmap::Vec2<int> shape_preview = {128, 128};
  int             preview_type = preview_type::grayscale;
  GLuint          image_texture_preview = 0;
  void            init_from_control_node();
};

class ViewControlNode : public ViewNode,
                        public gnode::Node // used to recast derived ViewNode
{
public:
  ViewControlNode(std::string id) : ViewNode(), gnode::Node(id)
  {
    this->set_p_control_node((gnode::Node *)this);
  }
};

//

class ViewAlterElevation : public ViewNode, public hesiod::cnode::AlterElevation
{
public:
  ViewAlterElevation(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewBaseElevation : public ViewNode, public hesiod::cnode::BaseElevation
{
public:
  ViewBaseElevation(std::string     id,
                    hmap::Vec2<int> shape,
                    hmap::Vec2<int> tiling,
                    float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewBezierPath : public ViewNode, public hesiod::cnode::BezierPath
{
public:
  ViewBezierPath(std::string id);

  void render_node_specific_content();

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewBlend : public ViewNode, public hesiod::cnode::Blend
{
public:
  ViewBlend(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewBrush : public ViewNode, public hesiod::cnode::Brush
{
public:
  ViewBrush(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewBump : public ViewNode, public hesiod::cnode::Bump
{
public:
  ViewBump(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewCaldera : public ViewNode, public hesiod::cnode::Caldera
{
public:
  ViewCaldera(std::string     id,
              hmap::Vec2<int> shape,
              hmap::Vec2<int> tiling,
              float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewCheckerboard : public ViewNode, public hesiod::cnode::Checkerboard
{
public:
  ViewCheckerboard(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  bool link_kxy = true;
};

class ViewClamp : public ViewNode, public hesiod::cnode::Clamp
{
public:
  ViewClamp(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewClone : public ViewNode, public hesiod::cnode::Clone
{
public:
  ViewClone(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &);
  void serialize_load(cereal::JSONInputArchive &);
};

class ViewCloud : public ViewNode, public hesiod::cnode::Cloud
{
public:
  ViewCloud(std::string id);

  void render_node_specific_content();

  bool render_settings();
};

class ViewCloudToArrayInterp : public ViewNode,
                               public hesiod::cnode::CloudToArrayInterp
{
public:
  ViewCloudToArrayInterp(std::string     id,
                         hmap::Vec2<int> shape,
                         hmap::Vec2<int> tiling,
                         float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewConvolveSVD : public ViewNode, public hesiod::cnode::ConvolveSVD
{
public:
  ViewConvolveSVD(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewCubicPulseTruncated : public ViewNode,
                                public hesiod::cnode::CubicPulseTruncated
{
public:
  ViewCubicPulseTruncated(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewDebug : public ViewNode, public hesiod::cnode::Debug
{
public:
  std::string log = "";
  bool        auto_export_png = false;

  ViewDebug(std::string id);

  void export_to_png();

  void post_control_node_update();

  bool render_settings();

  void render_node_specific_content();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewDepressionFilling : public ViewNode,
                              public hesiod::cnode::DepressionFilling
{
public:
  ViewDepressionFilling(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewDigPath : public ViewNode, public hesiod::cnode::DigPath
{
public:
  ViewDigPath(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewDistanceTransform : public ViewNode,
                              public hesiod::cnode::DistanceTransform
{
public:
  ViewDistanceTransform(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  int shape_working_choice = 1;
};

class ViewEqualize : public ViewNode, public hesiod::cnode::Equalize
{
public:
  ViewEqualize(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewErosionMaps : public ViewNode, public hesiod::cnode::ErosionMaps
{
public:
  ViewErosionMaps(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewExpandShrink : public ViewNode, public hesiod::cnode::ExpandShrink
{
public:
  ViewExpandShrink(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewExpandShrinkDirectional
    : public ViewNode,
      public hesiod::cnode::ExpandShrinkDirectional
{
public:
  ViewExpandShrinkDirectional(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewExport : public ViewNode, public hesiod::cnode::Export
{
public:
  ViewExport(std::string id);

  bool render_settings();

  void render_node_specific_content();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

protected:
  std::map<std::string, int> format_map = {
      {"png (8 bit)", hesiod::cnode::export_type::png8bit},
      {"png (16 bit)", hesiod::cnode::export_type::png16bit}
      // {"raw", hesiod::cnode::export_type::raw}
  };
};

class ViewFbmPerlin : public ViewNode, public hesiod::cnode::FbmPerlin
{
public:
  ViewFbmPerlin(std::string     id,
                hmap::Vec2<int> shape,
                hmap::Vec2<int> tiling,
                float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  bool link_kxy = true;
};

class ViewFbmWorley : public ViewNode, public hesiod::cnode::FbmWorley
{
public:
  ViewFbmWorley(std::string     id,
                hmap::Vec2<int> shape,
                hmap::Vec2<int> tiling,
                float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  bool link_kxy = true;
};

class ViewFractalizePath : public ViewNode, public hesiod::cnode::FractalizePath
{
public:
  ViewFractalizePath(std::string id);

  void render_node_specific_content();

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewGaborNoise : public ViewNode, public hesiod::cnode::GaborNoise
{
public:
  ViewGaborNoise(std::string     id,
                 hmap::Vec2<int> shape,
                 hmap::Vec2<int> tiling,
                 float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewGain : public ViewNode, public hesiod::cnode::Gain
{
public:
  ViewGain(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewGammaCorrection : public ViewNode,
                            public hesiod::cnode::GammaCorrection
{
public:
  ViewGammaCorrection(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewGammaCorrectionLocal : public ViewNode,
                                 public hesiod::cnode::GammaCorrectionLocal
{
public:
  ViewGammaCorrectionLocal(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewGradient : public ViewNode, public hesiod::cnode::Gradient
{
public:
  ViewGradient(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewGradientAngle : public ViewNode, public hesiod::cnode::GradientAngle
{
public:
  ViewGradientAngle(std::string id);

  void serialize_save(cereal::JSONOutputArchive &);
  void serialize_load(cereal::JSONInputArchive &);
};

class ViewGradientNorm : public ViewNode, public hesiod::cnode::GradientNorm
{
public:
  ViewGradientNorm(std::string id);

  void serialize_save(cereal::JSONOutputArchive &);
  void serialize_load(cereal::JSONInputArchive &);
};

class ViewGradientTalus : public ViewNode, public hesiod::cnode::GradientTalus
{
public:
  ViewGradientTalus(std::string id);

  void serialize_save(cereal::JSONOutputArchive &);
  void serialize_load(cereal::JSONInputArchive &);
};

class ViewGaussianPulse : public ViewNode, public hesiod::cnode::GaussianPulse
{
public:
  ViewGaussianPulse(std::string     id,
                    hmap::Vec2<int> shape,
                    hmap::Vec2<int> tiling,
                    float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewHydraulicAlgebric : public ViewNode,
                              public hesiod::cnode::HydraulicAlgebric
{
public:
  ViewHydraulicAlgebric(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewHydraulicBenes : public ViewNode, public hesiod::cnode::HydraulicBenes
{
public:
  ViewHydraulicBenes(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewHydraulicParticle : public ViewNode,
                              public hesiod::cnode::HydraulicParticle
{
public:
  ViewHydraulicParticle(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewHydraulicRidge : public ViewNode, public hesiod::cnode::HydraulicRidge
{
public:
  ViewHydraulicRidge(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewHydraulicStream : public ViewNode,
                            public hesiod::cnode::HydraulicStream
{
public:
  ViewHydraulicStream(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewHydraulicVpipes : public ViewNode,
                            public hesiod::cnode::HydraulicVpipes
{
public:
  ViewHydraulicVpipes(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewImport : public ViewNode, public hesiod::cnode::Import
{
public:
  ViewImport(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  bool render_settings();

  void render_node_specific_content();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewKernel : public ViewNode, public hesiod::cnode::Kernel
{
public:
  ViewKernel(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewKmeansClustering2 : public ViewNode,
                              public hesiod::cnode::KmeansClustering2
{
public:
  ViewKmeansClustering2(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  int shape_clustering_choice = 1;
};

class ViewLaplace : public ViewNode, public hesiod::cnode::Laplace
{
public:
  ViewLaplace(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewLaplaceEdgePreserving : public ViewNode,
                                  public hesiod::cnode::LaplaceEdgePreserving
{
public:
  ViewLaplaceEdgePreserving(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewLerp : public ViewNode, public hesiod::cnode::Lerp
{
public:
  ViewLerp(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewMakeBinary : public ViewNode, public hesiod::cnode::MakeBinary

{
public:
  ViewMakeBinary(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewMeanderizePath : public ViewNode, public hesiod::cnode::MeanderizePath
{
public:
  ViewMeanderizePath(std::string id);

  void render_node_specific_content();

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewMedian3x3 : public ViewNode, public hesiod::cnode::Median3x3
{
public:
  ViewMedian3x3(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewMinimumLocal : public ViewNode, public hesiod::cnode::MinimumLocal
{
public:
  ViewMinimumLocal(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewNormalDisplacement : public ViewNode,
                               public hesiod::cnode::NormalDisplacement
{
public:
  ViewNormalDisplacement(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewOneMinus : public ViewNode, public hesiod::cnode::OneMinus
{
public:
  ViewOneMinus(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewPath : public ViewNode, public hesiod::cnode::Path
{
public:
  ViewPath(std::string id);

  void render_node_specific_content();

  bool render_settings();
};

class ViewPathFinding : public ViewNode, public hesiod::cnode::PathFinding
{
public:
  ViewPathFinding(std::string id);

  void render_node_specific_content();

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  int wshape_choice = 1;
};

class ViewPathToHeightmap : public ViewNode,
                            public hesiod::cnode::PathToHeightmap
{
public:
  ViewPathToHeightmap(std::string     id,
                      hmap::Vec2<int> shape,
                      hmap::Vec2<int> tiling,
                      float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewPerlin : public ViewNode, public hesiod::cnode::Perlin
{
public:
  ViewPerlin(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  bool link_kxy = true;
};

class ViewPerlinBillow : public ViewNode, public hesiod::cnode::PerlinBillow
{
public:
  ViewPerlinBillow(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  bool link_kxy = true;
};

class ViewPlateau : public ViewNode, public hesiod::cnode::Plateau
{
public:
  ViewPlateau(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewPreview : public ViewNode, public hesiod::cnode::Preview
{
public:
  ViewPreview(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &);
  void serialize_load(cereal::JSONInputArchive &);
};

class ViewRecastCanyon : public ViewNode, public hesiod::cnode::RecastCanyon
{
public:
  ViewRecastCanyon(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &);
  void serialize_load(cereal::JSONInputArchive &);
};

class ViewRecurve : public ViewNode, public hesiod::cnode::Recurve
{
public:
  ViewRecurve(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewRecurveKura : public ViewNode, public hesiod::cnode::RecurveKura
{
public:
  ViewRecurveKura(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewRelativeElevation : public ViewNode,
                              public hesiod::cnode::RelativeElevation
{
public:
  ViewRelativeElevation(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewRemap : public ViewNode, public hesiod::cnode::Remap
{
public:
  ViewRemap(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewRidgedPerlin : public ViewNode, public hesiod::cnode::RidgedPerlin
{
public:
  ViewRidgedPerlin(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  bool link_kxy = true;
};

class ViewRugosity : public ViewNode, public hesiod::cnode::Rugosity
{
public:
  ViewRugosity(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewSedimentDeposition : public ViewNode,
                               public hesiod::cnode::SedimentDeposition
{
public:
  ViewSedimentDeposition(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewSelectCavities : public ViewNode, public hesiod::cnode::SelectCavities
{
public:
  ViewSelectCavities(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewSelectEq : public ViewNode, public hesiod::cnode::SelectEq
{
public:
  ViewSelectEq(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  bool               use_input_unique_values = false;
  std::vector<float> input_unique_values = {};
  int                selected_idx = 0;
};

class ViewSelectTransitions : public ViewNode,
                              public hesiod::cnode::SelectTransitions
{
public:
  ViewSelectTransitions(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewSlope : public ViewNode, public hesiod::cnode::Slope
{
public:
  ViewSlope(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewSmoothCpulse : public ViewNode, public hesiod::cnode::SmoothCpulse
{
public:
  ViewSmoothCpulse(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewSmoothFill : public ViewNode, public hesiod::cnode::SmoothFill
{
public:
  ViewSmoothFill(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewSmoothFillHoles : public ViewNode,
                            public hesiod::cnode::SmoothFillHoles
{
public:
  ViewSmoothFillHoles(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewSmoothFillSmearPeaks : public ViewNode,
                                 public hesiod::cnode::SmoothFillSmearPeaks
{
public:
  ViewSmoothFillSmearPeaks(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewSteepenConvective : public ViewNode,
                              public hesiod::cnode::SteepenConvective
{
public:
  ViewSteepenConvective(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewStep : public ViewNode, public hesiod::cnode::Step
{
public:
  ViewStep(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewStratifyMultiscale : public ViewNode,
                               public hesiod::cnode::StratifyMultiscale
{
public:
  ViewStratifyMultiscale(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewStratifyOblique : public ViewNode,
                            public hesiod::cnode::StratifyOblique
{
public:
  ViewStratifyOblique(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewThermal : public ViewNode, public hesiod::cnode::Thermal
{
public:
  ViewThermal(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewThermalAutoBedrock : public ViewNode,
                               public hesiod::cnode::ThermalAutoBedrock
{
public:
  ViewThermalAutoBedrock(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewThermalScree : public ViewNode, public hesiod::cnode::ThermalScree
{
public:
  ViewThermalScree(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewValleyWidth : public ViewNode, public hesiod::cnode::ValleyWidth
{
public:
  ViewValleyWidth(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewValueNoiseDelaunay : public ViewNode,
                               public hesiod::cnode::ValueNoiseDelaunay
{
public:
  ViewValueNoiseDelaunay(std::string     id,
                         hmap::Vec2<int> shape,
                         hmap::Vec2<int> tiling,
                         float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewValueNoiseLinear : public ViewNode,
                             public hesiod::cnode::ValueNoiseLinear
{
public:
  ViewValueNoiseLinear(std::string     id,
                       hmap::Vec2<int> shape,
                       hmap::Vec2<int> tiling,
                       float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  bool link_kxy = true;
};

class ViewWarp : public ViewNode, public hesiod::cnode::Warp
{
public:
  ViewWarp(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewWarpDownslope : public ViewNode, public hesiod::cnode::WarpDownslope
{
public:
  ViewWarpDownslope(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewWaveSine : public ViewNode, public hesiod::cnode::WaveSine
{
public:
  ViewWaveSine(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewWaveDune : public ViewNode, public hesiod::cnode::WaveDune
{
public:
  ViewWaveDune(std::string     id,
               hmap::Vec2<int> shape,
               hmap::Vec2<int> tiling,
               float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewWhite : public ViewNode, public hesiod::cnode::White
{
public:
  ViewWhite(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewWhiteDensityMap : public ViewNode,
                            public hesiod::cnode::WhiteDensityMap
{
public:
  ViewWhiteDensityMap(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewWhiteSparse : public ViewNode, public hesiod::cnode::WhiteSparse
{
public:
  ViewWhiteSparse(std::string     id,
                  hmap::Vec2<int> shape,
                  hmap::Vec2<int> tiling,
                  float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

class ViewWorley : public ViewNode, public hesiod::cnode::Worley
{
public:
  ViewWorley(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  bool link_kxy = true;
};

class ViewWorleyDouble : public ViewNode, public hesiod::cnode::WorleyDouble
{
public:
  ViewWorleyDouble(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  bool link_kxy = true;
};

class ViewWorleyValue : public ViewNode, public hesiod::cnode::WorleyValue
{
public:
  ViewWorleyValue(std::string     id,
                  hmap::Vec2<int> shape,
                  hmap::Vec2<int> tiling,
                  float           overlap);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);

private:
  bool link_kxy = true;
};

class ViewZeroedEdges : public ViewNode, public hesiod::cnode::ZeroedEdges
{
public:
  ViewZeroedEdges(std::string id);

  bool render_settings();

  void serialize_save(cereal::JSONOutputArchive &ar);
  void serialize_load(cereal::JSONInputArchive &ar);
};

// // HELPERS

void img_to_texture(std::vector<uint8_t> img,
                    hmap::Vec2<int>      shape,
                    GLuint              &image_texture);

void img_to_texture_rgb(std::vector<uint8_t> img,
                        hmap::Vec2<int>      shape,
                        GLuint              &image_texture);

void post_update_callback_wrapper(ViewNode *p_vnode, gnode::Node *p_cnode);

} // namespace hesiod::vnode
