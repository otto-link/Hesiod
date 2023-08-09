/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "imgui.h"

#include "hesiod/control_node.hpp"

namespace hesiod::vnode
{

class ViewNode
{
public:
  bool        auto_update;
  std::string id;
  std::string label;

  ViewNode(gnode::Node *p_control_node);

  virtual bool render_settings()
  {
    LOG_ERROR("Settings rendering not defined for generic node GUI [%s])",
              this->id.c_str());
    throw std::runtime_error("undefined 'render' method");
  }

  void render_node();

  bool render_settings_header();

  bool render_settings_footer();

private:
  gnode::Node *p_control_node;
  float        node_width = 128.f;
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

} // namespace hesiod::vnode
