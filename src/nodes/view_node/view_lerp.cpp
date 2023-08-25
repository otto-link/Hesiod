/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewLerp::ViewLerp(std::string id) : ViewNode(), hesiod::cnode::Lerp(id)
{
  LOG_DEBUG("hash_id: %d", this->hash_id);
  LOG_DEBUG("label: %s", this->label.c_str());
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewLerp::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  if (!this->get_p_data("t"))
  {
    ImGui::SliderFloat("t", &this->t, 0.f, 1.f, "%.2f");
    has_changed |= this->trigger_update_after_edit();
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
