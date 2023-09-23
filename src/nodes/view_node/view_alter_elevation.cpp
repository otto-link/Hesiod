/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewAlterElevation::ViewAlterElevation(std::string id)
    : ViewNode(), hesiod::cnode::AlterElevation(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewAlterElevation::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderInt("ir", &this->ir, 1, 128);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("footprint_ratio",
                     &this->footprint_ratio,
                     0.01f,
                     4.f,
                     "%.2f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
