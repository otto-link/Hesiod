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

ViewCheckerboard::ViewCheckerboard(std::string     id,
                                   hmap::Vec2<int> shape,
                                   hmap::Vec2<int> tiling,
                                   float           overlap)
    : ViewNode(), hesiod::cnode::Checkerboard(id, shape, tiling, overlap)
{
  LOG_DEBUG("ViewCheckerboard::ViewCheckerboard()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewCheckerboard::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

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