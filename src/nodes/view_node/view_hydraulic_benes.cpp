/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewHydraulicBenes::ViewHydraulicBenes(std::string id)
    : ViewNode(), hesiod::cnode::HydraulicBenes(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewHydraulicBenes::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::InputInt("iterations", &this->iterations);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("c_capacity", &this->c_capacity, 0.1f, 100.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("c_erosion", &this->c_erosion, 0.001f, 1.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("c_deposition", &this->c_deposition, 0.001f, 1.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("water_level", &this->water_level, 0.f, 0.1f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("evap_rate", &this->evap_rate, 0.f, 0.1f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("rain_rate", &this->rain_rate, 0.f, 0.1f);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewHydraulicBenes::serialize_save(cereal::JSONOutputArchive &ar)
{
  // ar(cereal::make_nvp("", this->));
}

void ViewHydraulicBenes::serialize_load(cereal::JSONInputArchive &ar)
{
}

} // namespace hesiod::vnode
