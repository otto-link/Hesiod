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
    this->seed = (int)time(NULL);
    this->force_update();
    has_changed = true;
  }

  ImGui::DragInt("seed", &this->seed);
  has_changed |= this->trigger_update_after_edit();

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

  ImGui::Separator();

  if (hesiod::gui::slider_vmin_vmax(vmin, vmax))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewValueNoiseDelaunay::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("kw", this->kw));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewValueNoiseDelaunay::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("kw", this->kw));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
