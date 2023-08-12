/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
// #include "GL/gl.h"
#include <string>

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
  bool        auto_update;
  std::string id;
  std::string label;

  ViewNode(gnode::Node *p_control_node);

  gnode::Node *get_p_control_node();

  void set_preview_port_id(std::string new_port_id);

  void set_preview_type(int new_preview_type);

  void post_control_node_update();

  virtual bool render_settings();

  void render_node();

  bool render_settings_header();

  bool render_settings_footer();

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
};

class ViewGammaCorrection : public ViewNode
{
public:
  float gamma = 1.f;

  ViewGammaCorrection(hesiod::cnode::GammaCorrection *p_control_node);

  bool render_settings();

private:
  hesiod::cnode::GammaCorrection *p_control_node;
};

class ViewGradientNorm : public ViewNode
{
public:
  ViewGradientNorm(hesiod::cnode::GradientNorm *p_control_node);

private:
  hesiod::cnode::GradientNorm *p_control_node;
};

class ViewGradientTalus : public ViewNode
{
public:
  ViewGradientTalus(hesiod::cnode::GradientTalus *p_control_node);

private:
  hesiod::cnode::GradientTalus *p_control_node;
};

class ViewPerlin : public ViewNode
{
public:
  hmap::Vec2<int>   shape;
  hmap::Vec2<float> kw;
  int               seed;
  float             vmin;
  float             vmax;

  ViewPerlin(hesiod::cnode::Perlin *p_control_node);

  bool render_settings();

private:
  hesiod::cnode::Perlin *p_control_node;
  bool                   link_kxy = true;
};

class ViewRemap : public ViewNode
{
public:
  float vmin;
  float vmax;

  ViewRemap(hesiod::cnode::Remap *p_control_node);

  bool render_settings();

private:
  hesiod::cnode::Remap *p_control_node;
};

class ViewWhiteDensityMap : public ViewNode
{
public:
  int   seed;
  float vmin;
  float vmax;

  ViewWhiteDensityMap(hesiod::cnode::WhiteDensityMap *p_control_node);

  bool render_settings();

private:
  hesiod::cnode::WhiteDensityMap *p_control_node;
};

// HELPERS

void img_to_texture(std::vector<uint8_t> img,
                    hmap::Vec2<int>      shape,
                    GLuint              &image_texture);

void post_update_callback_wrapper(ViewNode *p_vnode, gnode::Node *p_cnode);

} // namespace hesiod::vnode
