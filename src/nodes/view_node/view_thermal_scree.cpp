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
    : ViewNode(), hesiod::cnode::ThermalScree(id)
{
  LOG_DEBUG("ViewThermalScree::ViewThermalScree()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
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

void ViewThermalScree::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("talus_global", this->talus_global));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("zmax", this->zmax));
  ar(cereal::make_nvp("zmin", this->zmin));
  ar(cereal::make_nvp("noise_ratio", this->noise_ratio));
  ar(cereal::make_nvp("landing_talus_ratio", this->landing_talus_ratio));
  ar(cereal::make_nvp("landing_width_ratio", this->landing_width_ratio));
  ar(cereal::make_nvp("talus_constraint", this->talus_constraint));
}

void ViewThermalScree::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("talus_global", this->talus_global));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("zmax", this->zmax));
  ar(cereal::make_nvp("zmin", this->zmin));
  ar(cereal::make_nvp("noise_ratio", this->noise_ratio));
  ar(cereal::make_nvp("landing_talus_ratio", this->landing_talus_ratio));
  ar(cereal::make_nvp("landing_width_ratio", this->landing_width_ratio));
  ar(cereal::make_nvp("talus_constraint", this->talus_constraint));
}

} // namespace hesiod::vnode
