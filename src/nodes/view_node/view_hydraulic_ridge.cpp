/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewHydraulicRidge::ViewHydraulicRidge(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id),
      hesiod::cnode::HydraulicRidge(id)
{
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewHydraulicRidge::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("talus_global", &this->talus_global, 0.01f, 32.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("intensity", &this->intensity, 0.f, 2.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("erosion_factor", &this->erosion_factor, 0.f, 4.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("smoothing_factor", &this->smoothing_factor, 0.01f, 2.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("noise_ratio", &this->noise_ratio, 0.f, 1.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("ir", &this->ir, 0, 128);
  has_changed |= this->trigger_update_after_edit();

  ImGui::DragInt("seed", &this->seed);
  has_changed |= this->trigger_update_after_edit();

  if (ImGui::Button("Re-seed"))
  {
    this->seed = (int)time(NULL);
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
