/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewHydraulicStream::ViewHydraulicStream(std::string id)
    : ViewNode(), hesiod::cnode::HydraulicStream(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewHydraulicStream::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("c_erosion", &this->c_erosion, 0.001f, 0.1f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("talus_ref", &this->talus_ref, 0.001f, 10.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("ir", &this->ir, 1, 16);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("clipping_ratio", &this->clipping_ratio, 0.1f, 100.f);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewHydraulicStream::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("c_erosion", this->c_erosion));
  ar(cereal::make_nvp("talus_ref", this->talus_ref));
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("clipping_ratio", this->clipping_ratio));
}

void ViewHydraulicStream::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("c_erosion", this->c_erosion));
  ar(cereal::make_nvp("talus_ref", this->talus_ref));
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("clipping_ratio", this->clipping_ratio));
}

} // namespace hesiod::vnode
