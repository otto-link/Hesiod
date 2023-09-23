/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewHydraulicVpipes::ViewHydraulicVpipes(std::string id)
    : ViewNode(), hesiod::cnode::HydraulicVpipes(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewHydraulicVpipes::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::InputInt("iterations", &this->iterations);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("water_height", &this->water_height, 0.001f, 0.5f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("c_capacity", &this->c_capacity, 0.001f, 0.5f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("c_erosion", &this->c_erosion, 0.f, 0.5f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("c_deposition", &this->c_deposition, 0.f, 0.5f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("rain_rate", &this->rain_rate, 0.f, 0.1f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("evap_rate", &this->evap_rate, 0.001f, 0.1f);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewHydraulicVpipes::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("iterations", this->iterations));
  ar(cereal::make_nvp("water_height", this->water_height));
  ar(cereal::make_nvp("c_capacity", this->c_capacity));
  ar(cereal::make_nvp("c_erosion", this->c_erosion));
  ar(cereal::make_nvp("c_deposition", this->c_deposition));
  ar(cereal::make_nvp("rain_rate", this->rain_rate));
  ar(cereal::make_nvp("evap_rate", this->evap_rate));
}

void ViewHydraulicVpipes::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("iterations", this->iterations));
  ar(cereal::make_nvp("water_height", this->water_height));
  ar(cereal::make_nvp("c_capacity", this->c_capacity));
  ar(cereal::make_nvp("c_erosion", this->c_erosion));
  ar(cereal::make_nvp("c_deposition", this->c_deposition));
  ar(cereal::make_nvp("rain_rate", this->rain_rate));
  ar(cereal::make_nvp("evap_rate", this->evap_rate));
}

} // namespace hesiod::vnode
