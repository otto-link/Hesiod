/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewSmoothCpulse::ViewSmoothCpulse(hesiod::cnode::SmoothCpulse *p_control_node)
    : ViewNode((gnode::Node *)p_control_node), p_control_node(p_control_node)
{
  this->ir = p_control_node->get_ir();
  this->set_preview_port_id("output");
}

bool ViewSmoothCpulse::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  if (ImGui::SliderInt("int", &this->ir, 1, 256))
  {
    this->p_control_node->set_ir(this->ir);
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
