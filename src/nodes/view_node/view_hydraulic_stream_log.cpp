/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewHydraulicStreamLog::ViewHydraulicStreamLog(std::string id)
    : ViewNode(), hesiod::cnode::HydraulicStreamLog(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewHydraulicStreamLog::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("c_erosion", &this->c_erosion, 0.001f, 0.1f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("talus_ref", &this->talus_ref, 0.001f, 10.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("gamma", &this->gamma, 0.01f, 4.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("ir", &this->ir, 1, 16);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewHydraulicStreamLog::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("c_erosion", this->c_erosion));
  ar(cereal::make_nvp("talus_ref", this->talus_ref));
  ar(cereal::make_nvp("gamma", this->gamma));
  ar(cereal::make_nvp("ir", this->ir));
}

void ViewHydraulicStreamLog::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("c_erosion", this->c_erosion));
  ar(cereal::make_nvp("talus_ref", this->talus_ref));
  ar(cereal::make_nvp("gamma", this->gamma));
  ar(cereal::make_nvp("ir", this->ir));
}

} // namespace hesiod::vnode
