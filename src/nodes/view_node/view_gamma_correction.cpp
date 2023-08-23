/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewGammaCorrection::ViewGammaCorrection(std::string id)
    : ViewNode(), hesiod::cnode::GammaCorrection(id)
{
  LOG_DEBUG("hash_id: %d", this->hash_id);
  LOG_DEBUG("label: %s", this->label.c_str());
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewGammaCorrection::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  if (ImGui::SliderFloat("gamma", &this->gamma, 0.01f, 10.f, "%.2f"))
  {
    this->GammaCorrection::force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
