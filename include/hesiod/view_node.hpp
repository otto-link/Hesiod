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

class ViewGammaCorrection : public ViewNode,
                            public hesiod::cnode::GammaCorrection
{
public:
  ViewGammaCorrection(std::string id);

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
