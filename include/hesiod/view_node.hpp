/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "gnode.hpp"
#include <GLFW/glfw3.h>
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
    this->base = IM_COL32(rgba.a, rgba.b, rgba.c, (int)(0.3f * rgba.d));
    this->hovered = IM_COL32(rgba.a, rgba.b, rgba.c, (int)(0.7f * rgba.d));
    this->selected = IM_COL32(rgba.a, rgba.b, rgba.c, rgba.d);
  }
};

static const std::map<std::string, viewnode_color_set> category_colors = {
    {"Biome", viewnode_color_set({83, 147, 127, 255})},
    {"Debug", viewnode_color_set({200, 0, 0, 255})},
    {"Erosion", viewnode_color_set({58, 46, 26, 255})},
    {"Features", viewnode_color_set({244, 109, 67, 255})},
    {"Filter", viewnode_color_set({94, 79, 162, 255})},
    {"Hydrology", viewnode_color_set({101, 176, 234, 255})},
    {"Math", viewnode_color_set({20, 20, 20, 255})},
    {"Operator", viewnode_color_set({122, 136, 189, 255})},
    {"Primitive", viewnode_color_set({12, 84, 92, 255})},
    {"Roads", viewnode_color_set({102, 108, 111, 255})}};

class ViewNode
{
public:
  ViewNode();

  ViewNode(gnode::Node *p_control_node);

  gnode::Node *get_p_control_node();

  void set_p_control_node(gnode::Node *new_p_control_node);

  void set_preview_port_id(std::string new_port_id);

  void set_preview_type(int new_preview_type);

  virtual void post_control_node_update();

  virtual bool render_settings();

  void render_node();

  bool render_settings_header();

  bool render_settings_footer();

  bool render_view2d(std::string port_id);

  bool trigger_update_after_edit();

  void update_preview();

protected:
  std::string preview_port_id = "";

private:
  gnode::Node    *p_control_node;
  float           node_width = 128.f;
  bool            show_preview = true;
  hmap::Vec2<int> shape_preview = {128, 128};
  int             preview_type = preview_type::grayscale;
  GLuint          image_texture = 0;

  void init_from_control_node();
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

private:
  bool link_kxy = true;
};

class ViewGain : public ViewNode, public hesiod::cnode::Gain
{
public:
  ViewGain(std::string id);

  bool render_settings();
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

class ViewHydraulicParticle : public ViewNode,
                              public hesiod::cnode::HydraulicParticle
{
public:
  ViewHydraulicParticle(std::string id);

  bool render_settings();
};

class ViewKmeansClustering2 : public ViewNode,
                              public hesiod::cnode::KmeansClustering2
{
public:
  ViewKmeansClustering2(std::string id);

  bool render_settings();
};

class ViewLerp : public ViewNode, public hesiod::cnode::Lerp
{
public:
  ViewLerp(std::string id);

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

class ViewRemap : public ViewNode, public hesiod::cnode::Remap
{
public:
  ViewRemap(std::string id);

  bool render_settings();
};

class ViewRugosity : public ViewNode, public hesiod::cnode::Rugosity
{
public:
  ViewRugosity(std::string id);

  bool render_settings();
};

class ViewSmoothCpulse : public ViewNode, public hesiod::cnode::SmoothCpulse
{
public:
  ViewSmoothCpulse(std::string id);

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

void post_update_callback_wrapper(ViewNode *p_vnode, gnode::Node *p_cnode);

} // namespace hesiod::vnode
