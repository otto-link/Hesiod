/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewRugosity::ViewRugosity(std::string id)
    : ViewNode(), hesiod::cnode::Rugosity(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("input");
  this->set_view3d_color_port_id("output");
}

bool ViewRugosity::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderInt("ir", &this->ir, 1, 128);
  has_changed |= this->trigger_update_after_edit();

  ImGui::Checkbox("clamp_max", &this->clamp_max);
  has_changed |= this->trigger_update_after_edit();

  if (this->clamp_max)
  {
    ImGui::SliderFloat("vc_max", &this->vc_max, 0.f, 10.f, "%.2f");
    has_changed |= this->trigger_update_after_edit();
  }

  ImGui::Separator();

  has_changed |= render_settings_mask(
      (hesiod::cnode::Mask *)this->get_p_control_node());

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewRugosity::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("clamp_max", this->clamp_max));
  ar(cereal::make_nvp("vc_max", this->vc_max));

  serialize_save_settings_mask(
      (hesiod::cnode::Mask *)this->get_p_control_node(),
      ar);
}

void ViewRugosity::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("clamp_max", this->clamp_max));
  ar(cereal::make_nvp("vc_max", this->vc_max));

  serialize_load_settings_mask(
      (hesiod::cnode::Mask *)this->get_p_control_node(),
      ar);
}

} // namespace hesiod::vnode
