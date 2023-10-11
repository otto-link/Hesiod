/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#define _USE_MATH_DEFINES
#include <cmath>

#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewWaveDune::ViewWaveDune(std::string     id,
                           hmap::Vec2<int> shape,
                           hmap::Vec2<int> tiling,
                           float           overlap)
    : ViewNode(), hesiod::cnode::WaveDune(id, shape, tiling, overlap)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewWaveDune::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("kw", &this->kw, 0.001f, 64.f, "%.1f");
  has_changed |= this->trigger_update_after_edit();

  float alpha = this->angle / 180.f * M_PI;
  ImGui::SliderAngle("angle", &alpha, -90.f, 90.f);
  this->angle = alpha / M_PI * 180.f;

  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("xtop", &this->xtop, 0.f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("xbottom", &this->xbottom, 0.f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderAngle("phase_shift", &phase_shift, -180.f, 180.f);
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

void ViewWaveDune::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("kw", this->kw));
  ar(cereal::make_nvp("angle", this->angle));
  ar(cereal::make_nvp("xtop", this->xtop));
  ar(cereal::make_nvp("xbottom", this->xbottom));
  ar(cereal::make_nvp("phase_shift", this->phase_shift));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewWaveDune::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("kw", this->kw));
  ar(cereal::make_nvp("angle", this->angle));
  ar(cereal::make_nvp("xtop", this->xtop));
  ar(cereal::make_nvp("xbottom", this->xbottom));
  ar(cereal::make_nvp("phase_shift", this->phase_shift));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
