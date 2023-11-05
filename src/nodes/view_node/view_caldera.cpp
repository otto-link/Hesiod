/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewCaldera::ViewCaldera(std::string     id,
                         hmap::Vec2<int> shape,
                         hmap::Vec2<int> tiling,
                         float           overlap)
    : ViewNode(), hesiod::cnode::Caldera(id, shape, tiling, overlap)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewCaldera::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("radius", &this->radius, 1.f, 512.f, "%.1f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("sigma_inner", &this->sigma_inner, 1.f, 512.f, "%.1f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("sigma_outer", &this->sigma_outer, 1.f, 512.f, "%.1f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("z_bottom", &this->z_bottom, -1.f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("noise_r_amp", &this->noise_r_amp, 1.f, 128.f, "%.1f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("noise_ratio_z", &this->noise_ratio_z, 0.f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("center.x", &this->center.x, -0.5f, 1.5f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("center.y", &this->center.y, -0.5f, 1.5f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::Separator();

  if (hesiod::gui::slider_vmin_vmax(vmin, vmax))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewCaldera::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("radius", this->radius));
  ar(cereal::make_nvp("sigma_inner", this->sigma_inner));
  ar(cereal::make_nvp("sigma_outer", this->sigma_outer));
  ar(cereal::make_nvp("z_bottom", this->z_bottom));
  ar(cereal::make_nvp("noise_r_amp", this->noise_r_amp));
  ar(cereal::make_nvp("noise_ratio_z", this->noise_ratio_z));
  ar(cereal::make_nvp("center.x", this->center.x));
  ar(cereal::make_nvp("cneter.y", this->center.y));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewCaldera::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("radius", this->radius));
  ar(cereal::make_nvp("sigma_inner", this->sigma_inner));
  ar(cereal::make_nvp("sigma_outer", this->sigma_outer));
  ar(cereal::make_nvp("z_bottom", this->z_bottom));
  ar(cereal::make_nvp("noise_r_amp", this->noise_r_amp));
  ar(cereal::make_nvp("noise_ratio_z", this->noise_ratio_z));
  ar(cereal::make_nvp("center.x", this->center.x));
  ar(cereal::make_nvp("cneter.y", this->center.y));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
