/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewGaussianPulse::ViewGaussianPulse(std::string     id,
                                     hmap::Vec2<int> shape,
                                     hmap::Vec2<int> tiling,
                                     float           overlap)
    : ViewNode(), hesiod::cnode::GaussianPulse(id, shape, tiling, overlap)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view2d_port_id("output");
}

bool ViewGaussianPulse::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::Checkbox("inverse", &this->inverse);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("sigma", &this->sigma, 1.f, 512.f, "%.1f");
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
