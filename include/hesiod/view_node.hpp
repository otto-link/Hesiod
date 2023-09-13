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
  uint32_t selected;

  viewnode_color_set(hmap::Vec4<int> rgba)
  {
    uint8_t dc = 50;

    this->base = IM_COL32(rgba.a, rgba.b, rgba.c, (int)(0.3f * rgba.d));
    this->hovered = IM_COL32(rgba.a, rgba.b, rgba.c, (int)(0.7f * rgba.d));
    this->selected = IM_COL32(std::min(255, rgba.a + dc),
                              std::min(255, rgba.b + dc),
                              std::min(255, rgba.c + dc),
                              rgba.d);
  }
};

static const std::map<std::string, viewnode_color_set> category_colors = {
    {"Biome", viewnode_color_set({83, 147, 127, 255})},
    {"Debug", viewnode_color_set({200, 0, 0, 255})},
    {"Erosion", viewnode_color_set({58, 46, 26, 255})},
    {"Features", viewnode_color_set({244, 109, 67, 255})},
    {"Filter", viewnode_color_set({94, 79, 162, 255})},
    {"Geometry", viewnode_color_set({255, 255, 255, 255})},
    {"Hydrology", viewnode_color_set({101, 176, 234, 255})},
    {"Math", viewnode_color_set({20, 20, 20, 255})},
    {"Mask", viewnode_color_set({0, 0, 255, 255})},
    {"Operator", viewnode_color_set({142, 156, 189, 255})},
    {"Primitive", viewnode_color_set({12, 84, 92, 255})},
    {"Roads", viewnode_color_set({102, 108, 111, 255})},
    {"Routing", viewnode_color_set({200, 50, 0, 255})}};

class ViewNode
{
public:
  ViewNode();

  ViewNode(gnode::Node *p_control_node);

  gnode::Node *get_p_control_node();

  void set_p_control_node(gnode::Node *new_p_control_node);

  void set_preview_port_id(std::string new_port_id);

  void set_preview_type(int new_preview_type);

  void set_show_view2d(bool new_show_view2d);

  void set_view2d_port_id(std::string new_port_id);

  virtual void post_control_node_update();

  virtual bool render_settings();

  void render_node();

  bool render_settings_header();

  bool render_settings_footer();

  bool render_view2d();

  virtual void serialize_load(cereal::JSONInputArchive &);
  virtual void serialize_save(cereal::JSONOutputArchive &);

  bool trigger_update_after_edit();

  void update_preview();

protected:
  std::string preview_port_id = "";
  std::string view2d_port_id = "";
  bool        show_preview = true;
  bool        show_view2d = false;

private:
  gnode::Node    *p_control_node;
  float           node_width = 128.f;
  hmap::Vec2<int> shape_preview = {128, 128};
  hmap::Vec2<int> shape_view2d = {128, 128};
  int             preview_type = preview_type::grayscale;
  GLuint          image_texture_preview = 0;
  GLuint          image_texture_view2d = 0;
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
};

class ViewBaseElevation : public ViewNode, public hesiod::cnode::BaseElevation
{
public:
  ViewBaseElevation(std::string     id,
                    hmap::Vec2<int> shape,
                    hmap::Vec2<int> tiling,
                    float           overlap);

  bool render_settings();
};

class ViewBlend : public ViewNode, public hesiod::cnode::Blend
{
public:
  ViewBlend(std::string id);

  bool render_settings();
};

class ViewBump : public ViewNode, public hesiod::cnode::Bump
{
public:
  ViewBump(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  bool render_settings();
};

class ViewCheckerboard : public ViewNode, public hesiod::cnode::Checkerboard
{
public:
  ViewCheckerboard(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap);

  bool render_settings();

private:
  bool link_kxy = true;
};

class ViewClamp : public ViewNode, public hesiod::cnode::Clamp
{
public:
  ViewClamp(std::string id);

  bool render_settings();
};

class ViewClone : public ViewNode, public hesiod::cnode::Clone
{
public:
  ViewClone(std::string id);

  bool render_settings();
};

class ViewCloud : public ViewNode, public hesiod::cnode::Cloud
{
public:
  ViewCloud(std::string id);

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
};

class ViewExpandShrink : public ViewNode, public hesiod::cnode::ExpandShrink
{
public:
  ViewExpandShrink(std::string id);

  bool render_settings();
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
};

class ViewGammaCorrectionLocal : public ViewNode,
                                 public hesiod::cnode::GammaCorrectionLocal
{
public:
  ViewGammaCorrectionLocal(std::string id);

  bool render_settings();
};

class ViewGradient : public ViewNode, public hesiod::cnode::Gradient
{
public:
  ViewGradient(std::string id);

  bool render_settings();
};

class ViewGradientNorm : public ViewNode, public hesiod::cnode::GradientNorm
{
public:
  ViewGradientNorm(std::string id);
};

class ViewGradientTalus : public ViewNode, public hesiod::cnode::GradientTalus
{
public:
  ViewGradientTalus(std::string id);
};

class ViewGaussianPulse : public ViewNode, public hesiod::cnode::GaussianPulse
{
public:
  ViewGaussianPulse(std::string     id,
                    hmap::Vec2<int> shape,
                    hmap::Vec2<int> tiling,
                    float           overlap);

  bool render_settings();
};

class ViewHydraulicParticle : public ViewNode,
                              public hesiod::cnode::HydraulicParticle
{
public:
  ViewHydraulicParticle(std::string id);

  bool render_settings();
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

class ViewKmeansClustering2 : public ViewNode,
                              public hesiod::cnode::KmeansClustering2
{
public:
  ViewKmeansClustering2(std::string id);

  bool render_settings();

private:
  int shape_clustering_choice = 1;
};

class ViewLerp : public ViewNode, public hesiod::cnode::Lerp
{
public:
  ViewLerp(std::string id);

  bool render_settings();
};

class ViewMakeBinary : public ViewNode, public hesiod::cnode::MakeBinary
{
public:
  ViewMakeBinary(std::string id);

  bool render_settings();
};

class ViewMinimumLocal : public ViewNode, public hesiod::cnode::MinimumLocal
{
public:
  ViewMinimumLocal(std::string id);

  bool render_settings();
};

class ViewPerlin : public ViewNode, public hesiod::cnode::Perlin
{
public:
  ViewPerlin(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  bool render_settings();

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

private:
  bool link_kxy = true;
};

class ViewRecurve : public ViewNode, public hesiod::cnode::Recurve
{
public:
  ViewRecurve(std::string id);

  bool render_settings();
};

class ViewRemap : public ViewNode, public hesiod::cnode::Remap
{
public:
  ViewRemap(std::string id);

  bool render_settings();
};

class ViewRidgedPerlin : public ViewNode, public hesiod::cnode::RidgedPerlin
{
public:
  ViewRidgedPerlin(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap);

  bool render_settings();

private:
  bool link_kxy = true;
};

class ViewRugosity : public ViewNode, public hesiod::cnode::Rugosity
{
public:
  ViewRugosity(std::string id);

  bool render_settings();
};

class ViewSelectEq : public ViewNode, public hesiod::cnode::SelectEq
{
public:
  ViewSelectEq(std::string id);

  bool render_settings();

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
};

class ViewSmoothCpulse : public ViewNode, public hesiod::cnode::SmoothCpulse
{
public:
  ViewSmoothCpulse(std::string id);

  bool render_settings();
};

class ViewSmoothFill : public ViewNode, public hesiod::cnode::SmoothFill
{
public:
  ViewSmoothFill(std::string id);

  bool render_settings();
};

class ViewSteepenConvective : public ViewNode,
                              public hesiod::cnode::SteepenConvective
{
public:
  ViewSteepenConvective(std::string id);

  bool render_settings();
};

class ViewValleyWidth : public ViewNode, public hesiod::cnode::ValleyWidth
{
public:
  ViewValleyWidth(std::string id);

  bool render_settings();
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
};

class ViewWarp : public ViewNode, public hesiod::cnode::Warp
{
public:
  ViewWarp(std::string id);

  bool render_settings();
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

class ViewWhite : public ViewNode, public hesiod::cnode::White
{
public:
  ViewWhite(std::string     id,
            hmap::Vec2<int> shape,
            hmap::Vec2<int> tiling,
            float           overlap);

  bool render_settings();
};

class ViewWhiteDensityMap : public ViewNode,
                            public hesiod::cnode::WhiteDensityMap
{
public:
  ViewWhiteDensityMap(std::string id);

  bool render_settings();
};

class ViewWorley : public ViewNode, public hesiod::cnode::Worley
{
public:
  ViewWorley(std::string     id,
             hmap::Vec2<int> shape,
             hmap::Vec2<int> tiling,
             float           overlap);

  bool render_settings();

private:
  bool link_kxy = true;
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
