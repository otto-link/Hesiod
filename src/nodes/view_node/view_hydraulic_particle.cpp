/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewHydraulicParticle::ViewHydraulicParticle(std::string id)
    : ViewNode(), hesiod::cnode::HydraulicParticle(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewHydraulicParticle::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::AlignTextToFramePadding();
  if (ImGui::Button("Re-seed"))
  {
    this->seed = (int)time(NULL);
    this->force_update();
    has_changed = true;
  }

  if (ImGui::DragInt("seed", &this->seed))
  {
    this->force_update();
    has_changed = true;
  }

  if (ImGui::DragInt("nparticles", &this->nparticles))
  {
    this->force_update();
    has_changed = true;
  }

  if (ImGui::SliderInt("c_radius", &this->c_radius, 0, 16))
  {
    this->force_update();
    has_changed = true;
  }

  if (ImGui::SliderFloat("c_capacity", &this->c_capacity, 0.1f, 100.f))
  {
    this->force_update();
    has_changed = true;
  }

  if (ImGui::SliderFloat("c_erosion", &this->c_erosion, 0.001f, 0.1f))
  {
    this->force_update();
    has_changed = true;
  }

  if (ImGui::SliderFloat("c_deposition", &this->c_deposition, 0.001f, 0.1f))
  {
    this->force_update();
    has_changed = true;
  }

  if (ImGui::SliderFloat("drag_rate", &this->drag_rate, 0.f, 1.f))
  {
    this->force_update();
    has_changed = true;
  }

  if (ImGui::SliderFloat("evap_rate", &this->evap_rate, 0.f, 0.1f))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
