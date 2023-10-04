/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewClamp::ViewClamp(std::string id) : ViewNode(), hesiod::cnode::Clamp(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewClamp::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("vmin", &this->vmin, -1.f, 2.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("vmax", &this->vmax, -1.f, 2.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::Checkbox("smooth min", &this->smooth_min);
  has_changed |= this->trigger_update_after_edit();
  if (this->smooth_min)
  {
    ImGui::SliderFloat("k_min", &this->k_min, 0.01f, 1.f, "%.2f");
    has_changed |= this->trigger_update_after_edit();
  }

  ImGui::Checkbox("smooth max", &this->smooth_max);
  has_changed |= this->trigger_update_after_edit();
  if (this->smooth_max)
  {
    ImGui::SliderFloat("k_max", &this->k_max, 0.01f, 1.f, "%.2f");
    has_changed |= this->trigger_update_after_edit();
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewClamp::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("vmin", vmin));
  ar(cereal::make_nvp("vmax", vmax));
  ar(cereal::make_nvp("smooth_min", smooth_min));
  ar(cereal::make_nvp("smooth_max", smooth_max));
  ar(cereal::make_nvp("k_min", k_min));
  ar(cereal::make_nvp("k_max", k_max));
}

void ViewClamp::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("vmin", vmin));
  ar(cereal::make_nvp("vmax", vmax));
  ar(cereal::make_nvp("smooth_min", smooth_min));
  ar(cereal::make_nvp("smooth_max", smooth_max));
  ar(cereal::make_nvp("k_min", k_min));
  ar(cereal::make_nvp("k_max", k_max));
}

} // namespace hesiod::vnode
