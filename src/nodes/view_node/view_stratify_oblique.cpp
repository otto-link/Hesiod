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

ViewStratifyOblique::ViewStratifyOblique(std::string id)
    : ViewNode(), hesiod::cnode::StratifyOblique(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewStratifyOblique::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  if (ImGui::Button("Re-seed"))
  {
    this->seed = (int)time(NULL);
    this->force_update();
    has_changed = true;
  }

  ImGui::DragInt("seed", &this->seed);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("n_strata", &this->n_strata, 1, 16);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("strata_noise", &this->strata_noise, 0.f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("gamma", &this->gamma, 0.01f, 10.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("gamma_noise", &this->gamma_noise, 0.f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  float alpha = this->angle / 180.f * M_PI;
  ImGui::SliderAngle("angle", &alpha, -90.f, 90.f);
  this->angle = alpha / M_PI * 180.f;

  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("talus_global", &this->talus_global, 0.f, 32.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewStratifyOblique::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("n_strata", this->n_strata));
  ar(cereal::make_nvp("strata_noise", this->strata_noise));
  ar(cereal::make_nvp("gamma", this->gamma));
  ar(cereal::make_nvp("gamma_noise", this->gamma_noise));
  ar(cereal::make_nvp("talus_global", this->talus_global));
  ar(cereal::make_nvp("angle", this->angle));
  ar(cereal::make_nvp("seed", this->seed));
}

void ViewStratifyOblique::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("n_strata", this->n_strata));
  ar(cereal::make_nvp("strata_noise", this->strata_noise));
  ar(cereal::make_nvp("gamma", this->gamma));
  ar(cereal::make_nvp("gamma_noise", this->gamma_noise));
  ar(cereal::make_nvp("talus_global", this->talus_global));
  ar(cereal::make_nvp("angle", this->angle));
  ar(cereal::make_nvp("seed", this->seed));
}

} // namespace hesiod::vnode
