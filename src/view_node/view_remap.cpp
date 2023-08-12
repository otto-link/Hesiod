/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewRemap::ViewRemap(hesiod::cnode::Remap *p_control_node)
    : ViewNode((gnode::Node *)p_control_node), p_control_node(p_control_node)
{
  this->vmin = p_control_node->get_vmin();
  this->vmax = p_control_node->get_vmax();
}

bool ViewRemap::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  // output range
  ImGui::Spacing();
  if (ImGui::DragFloatRange2("Range",
                             &this->vmin,
                             &this->vmax,
                             0.01f,
                             -FLT_MAX,
                             FLT_MAX,
                             "vmin: %.2f",
                             "vmax: %.2f",
                             ImGuiSliderFlags_AlwaysClamp))
  {
    this->p_control_node->set_vmin(this->vmin);
    this->p_control_node->set_vmax(this->vmax);
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
