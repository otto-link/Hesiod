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

ViewBaseElevation::ViewBaseElevation(std::string     id,
                                     hmap::Vec2<int> shape,
                                     hmap::Vec2<int> tiling,
                                     float           overlap)
    : ViewNode(), hesiod::cnode::BaseElevation(id, shape, tiling, overlap)
{
  LOG_DEBUG("ViewBaseElevation::ViewBaseElevation()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewBaseElevation::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("width_factor", &this->width_factor, 0.01f, 2.f);
  has_changed |= this->trigger_update_after_edit();

  if (hesiod::gui::drag_float_matrix(this->values))
  {
    this->BaseElevation::force_update();
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
