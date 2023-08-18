/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewRemap::ViewRemap(std::string id) : ViewNode(), hesiod::cnode::Remap(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
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
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
