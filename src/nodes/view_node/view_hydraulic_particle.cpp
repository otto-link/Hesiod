/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewHydraulicParticle::ViewHydraulicParticle(
    hesiod::cnode::HydraulicParticle *p_control_node)
    : ViewNode((gnode::Node *)p_control_node), p_control_node(p_control_node)
{
  this->seed = p_control_node->get_seed();
  this->nparticles = p_control_node->get_nparticles();
  this->c_radius = p_control_node->get_c_radius();
  this->c_capacity = p_control_node->get_c_capacity();
  this->c_erosion = p_control_node->get_c_erosion();
  this->c_deposition = p_control_node->get_c_deposition();
  this->drag_rate = p_control_node->get_drag_rate();
  this->evap_rate = p_control_node->get_evap_rate();
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
    this->p_control_node->set_seed((uint)this->seed);
    has_changed = true;
  }

  if (ImGui::DragInt("seed", &this->seed))
  {
    this->p_control_node->set_seed((uint)this->seed);
    has_changed = true;
  }

  if (ImGui::DragInt("nparticles", &this->nparticles))
  {
    this->p_control_node->set_nparticles(this->nparticles);
    has_changed = true;
  }

  if (ImGui::SliderInt("c_radius", &this->c_radius, 0, 16))
  {
    this->p_control_node->set_c_radius(this->c_radius);
    has_changed = true;
  }

  if (ImGui::SliderFloat("c_capacity", &this->c_capacity, 0.1f, 100.f))
  {
    this->p_control_node->set_c_capacity(this->c_capacity);
    has_changed = true;
  }

  if (ImGui::SliderFloat("c_erosion", &this->c_erosion, 0.001f, 1.f))
  {
    this->p_control_node->set_c_erosion(this->c_erosion);
    has_changed = true;
  }

  if (ImGui::SliderFloat("c_deposition", &this->c_deposition, 0.001f, 1.f))
  {
    this->p_control_node->set_c_deposition(this->c_deposition);
    has_changed = true;
  }

  if (ImGui::SliderFloat("drag_rate", &this->drag_rate, 0.f, 1.f))
  {
    this->p_control_node->set_drag_rate(this->drag_rate);
    has_changed = true;
  }

  if (ImGui::SliderFloat("evap_rate", &this->evap_rate, 0.f, 1.f))
  {
    this->p_control_node->set_evap_rate(this->evap_rate);
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
