/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui.h"
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewPerlin::ViewPerlin(hesiod::cnode::Perlin *p_control_node)
    : ViewNode((gnode::Node *)p_control_node), p_control_node(p_control_node)
{
  this->shape = p_control_node->get_shape();
  this->kw = p_control_node->get_kw();
  this->seed = p_control_node->get_seed();
  this->vmin = p_control_node->get_vmin();
  this->vmax = p_control_node->get_vmax();
  this->set_preview_port_id("output");
}

bool ViewPerlin::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::AlignTextToFramePadding();
  if (ImGui::Button("Re-seed"))
  {
    // TODO to do in the node itself
    this->seed = (int)time(NULL);
    this->p_control_node->set_seed((uint)this->seed);
    has_changed = true;
  }

  if (ImGui::DragInt("seed", &this->seed))
  {
    this->p_control_node->set_seed((uint)this->seed);
    has_changed = true;
  }

  // wavenumber(s)
  ImGui::Checkbox("Link x and y", &this->link_kxy);
  if (this->link_kxy)
  {
    if (ImGui::DragFloat("kx, ky",
                         &this->kw.x,
                         0.1f,
                         0.f,
                         64.f,
                         "%.1f",
                         ImGuiSliderFlags_None))
    {
      this->kw.y = this->kw.x;
      this->p_control_node->set_kw(this->kw);
      has_changed = true;
    }
  }
  else
  {
    ImVec2 kxy = {this->kw.x, this->kw.y};
    if (ImGui::DragFloat2("kx, ky",
                          (float *)&kxy,
                          0.1f,
                          0.f,
                          64.f,
                          "%.1f",
                          ImGuiSliderFlags_None))
    {
      this->kw = {kxy.x, kxy.y};
      this->p_control_node->set_kw(this->kw);
      has_changed = true;
    }
  }

  // output range
  ImGui::Spacing();
  if (ImGui::DragFloatRange2("Range",
                             &this->vmin,
                             &this->vmax,
                             0.01f,
                             -FLT_MAX,
                             FLT_MAX,
                             "vmin: %.2f",
                             "vmax: %.2f",
                             ImGuiSliderFlags_AlwaysClamp))
  {
    this->p_control_node->set_vmin(this->vmin);
    this->p_control_node->set_vmax(this->vmax);
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
