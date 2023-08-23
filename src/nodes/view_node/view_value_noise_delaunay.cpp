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

ViewValueNoiseDelaunay::ViewValueNoiseDelaunay(std::string     id,
                                               hmap::Vec2<int> shape,
                                               hmap::Vec2<int> tiling,
                                               float           overlap)
    : ViewNode(), hesiod::cnode::ValueNoiseDelaunay(id, shape, tiling, overlap)
{
  LOG_DEBUG("ViewValueNoiseDelaunay::ViewValueNoiseDelaunay()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewValueNoiseDelaunay::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::AlignTextToFramePadding();
  if (ImGui::Button("Re-seed"))
  {
    // TODO to do in the node itself
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
  if (ImGui::DragFloat("k",
                       &this->kw,
                       0.1f,
                       1.f,
                       64.f,
                       "%.1f",
                       ImGuiSliderFlags_None))
  {
    this->force_update();
    has_changed = true;
  }

  ImGui::Spacing();
  if (hesiod::gui::slider_vmin_vmax(vmin, vmax))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
