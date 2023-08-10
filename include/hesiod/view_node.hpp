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

  void set_preview_port_id(std::string new_port_id);

  void post_control_node_update()
  {
    LOG_DEBUG("post-update, node [%s]", this->id.c_str());

    if (this->preview_port_id != "")
      this->update_preview();
  }

  virtual bool render_settings()
  {
    LOG_ERROR("Settings rendering not defined for generic node GUI [%s])",
              this->id.c_str());
    throw std::runtime_error("undefined 'render' method");
  }

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
  ViewGammaCorrection(hesiod::cnode::GammaCorrection *p_control_node);

  bool render_settings();

private:
  hesiod::cnode::GammaCorrection *p_control_node;
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

void img_to_texture(std::vector<uint8_t> img,
                    hmap::Vec2<int>      shape,
                    GLuint              &image_texture);

void post_update_callback_wrapper(ViewNode *p_vnode, gnode::Node *p_cnode);

} // namespace hesiod::vnode
