/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewGammaCorrection::ViewGammaCorrection(
    hesiod::cnode::GammaCorrection *p_control_node)
    : ViewNode((gnode::Node *)p_control_node), p_control_node(p_control_node)
{
  this->set_preview_port_id("output");
}

bool ViewGammaCorrection::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  if (ImGui::SliderFloat("gamma", &this->gamma, 0.01f, 10.f))
  {
    this->p_control_node->set_gamma(this->gamma);
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
