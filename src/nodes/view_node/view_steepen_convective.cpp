/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewSteepenConvective::ViewSteepenConvective(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id),
      hesiod::cnode::SteepenConvective(id)
{
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewSteepenConvective::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  float alpha = this->angle / 180.f * M_PI;
  ImGui::SliderAngle("angle", &alpha, -90.f, 90.f);
  this->angle = alpha / M_PI * 180.f;
  has_changed |= this->trigger_update_after_edit();

  ImGui::InputInt("iterations", &this->iterations, 1, 100);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("ir", &this->ir, 0, 128);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("dt", &this->dt, 0.f, 2.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
