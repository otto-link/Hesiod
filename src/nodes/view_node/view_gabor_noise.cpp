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

ViewGaborNoise::ViewGaborNoise(std::string     id,
                               hmap::Vec2<int> shape,
                               hmap::Vec2<int> tiling,
                               float           overlap)
    : hesiod::cnode::ControlNode(id), ViewNode(id),
      hesiod::cnode::GaborNoise(id, shape, tiling, overlap)
{
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewGaborNoise::render_settings()
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

  ImGui::DragFloat("kw",
                   &this->kw,
                   0.01f,
                   0.01f,
                   64.f,
                   "%.2f",
                   ImGuiSliderFlags_None);
  has_changed |= this->trigger_update_after_edit();

  float alpha = this->angle / 180.f * M_PI;
  ImGui::SliderAngle("angle", &alpha, -90.f, 90.f);
  this->angle = alpha / M_PI * 180.f;
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("width", &this->width, 1, 256);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("density", &this->density, 0.f, 1.f, "%.2f");
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

} // namespace hesiod::vnode
