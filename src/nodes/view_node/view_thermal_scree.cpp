/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui.h"
#include "macrologger.h"

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewThermalScree::ViewThermalScree(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id),
      hesiod::cnode::ThermalScree(id)
{
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewThermalScree::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::Checkbox("talus_constraint", &this->talus_constraint);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("talus_global", &this->talus_global, 1.f, 8.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("zmin", &this->zmin, -1.f, 2.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("zmax", &this->zmax, -1.f, 2.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("noise_ratio", &this->noise_ratio, 0.f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("landing_talus_ratio",
                     &this->landing_talus_ratio,
                     0.f,
                     1.f,
                     "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("landing_width_ratio",
                     &this->landing_width_ratio,
                     0.01f,
                     1.f,
                     "%.2f");
  has_changed |= this->trigger_update_after_edit();

  if (ImGui::Button("Re-seed"))
  {
    this->seed = (int)time(NULL);
    this->force_update();
    has_changed = true;
  }

  ImGui::DragInt("seed", &this->seed);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
