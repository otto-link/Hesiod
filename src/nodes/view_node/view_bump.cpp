/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewBump::ViewBump(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap)
    : ViewNode(), hesiod::cnode::Bump(id, shape, tiling, overlap)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewBump::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("gain", &this->gain, 0.01f, 10.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::Checkbox("inverse", &this->inverse);
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

void ViewBump::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("gain", this->gain));
  ar(cereal::make_nvp("inverse", this->inverse));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewBump::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("gain", this->gain));
  ar(cereal::make_nvp("inverse", this->inverse));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
