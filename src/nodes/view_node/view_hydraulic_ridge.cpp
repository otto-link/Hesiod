/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewHydraulicRidge::ViewHydraulicRidge(std::string id)
    : ViewNode(), hesiod::cnode::HydraulicRidge(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewHydraulicRidge::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("talus", &this->talus, 0.001f, 10.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("intensity", &this->intensity, 0.f, 2.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("erosion_factor", &this->erosion_factor, 0.f, 4.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("smoothing_factor", &this->smoothing_factor, 0.01f, 2.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("noise_ratio", &this->noise_ratio, 0.f, 1.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("ir", &this->ir, 0, 16);
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

void ViewHydraulicRidge::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("talus", this->talus));
  ar(cereal::make_nvp("intensity", this->intensity));
  ar(cereal::make_nvp("erosion_factor", this->erosion_factor));
  ar(cereal::make_nvp("smoothing_factor", this->smoothing_factor));
  ar(cereal::make_nvp("noise_ratio", this->noise_ratio));
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("seed", this->seed));
}

void ViewHydraulicRidge::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("talus", this->talus));
  ar(cereal::make_nvp("intensity", this->intensity));
  ar(cereal::make_nvp("erosion_factor", this->erosion_factor));
  ar(cereal::make_nvp("smoothing_factor", this->smoothing_factor));
  ar(cereal::make_nvp("noise_ratio", this->noise_ratio));
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("seed", this->seed));
}

} // namespace hesiod::vnode
