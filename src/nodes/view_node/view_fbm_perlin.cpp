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

ViewFbmPerlin::ViewFbmPerlin(std::string     id,
                             hmap::Vec2<int> shape,
                             hmap::Vec2<int> tiling,
                             float           overlap)
    : ViewNode(), hesiod::cnode::FbmPerlin(id, shape, tiling, overlap)
{
  LOG_DEBUG("ViewFbmPerlin::ViewFbmPerlin()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewFbmPerlin::render_settings()
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
      this->force_update();
      has_changed = true;
    }
  }
  else
  {
    // if (ImGui::DragFloat2("kx, ky",
    //                       (float *) this->kw, // TODO
    //                       0.1f,
    //                       0.f,
    //                       64.f,
    //                       "%.1f",
    //                       ImGuiSliderFlags_None))
    // {
    //   this->force_update();
    //   has_changed = true;
    // }
  }

  if (ImGui::InputInt("octaves", &this->octaves))
  {
    this->force_update();
    has_changed = true;
  }

  if (ImGui::SliderFloat("weight", &this->weight, 0.f, 1.f, "%.2f"))
  {
    this->force_update();
    has_changed = true;
  }

  if (ImGui::SliderFloat("persistence", &this->persistence, 0.01f, 1.f, "%.2f"))
  {
    this->force_update();
    has_changed = true;
  }

  if (ImGui::SliderFloat("lacunarity", &this->lacunarity, 0.01f, 4.f, "%.2f"))
  {
    this->force_update();
    has_changed = true;
  }

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
