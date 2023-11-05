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

ViewWhiteSparse::ViewWhiteSparse(std::string     id,
                                 hmap::Vec2<int> shape,
                                 hmap::Vec2<int> tiling,
                                 float           overlap)
    : ViewNode(), hesiod::cnode::WhiteSparse(id, shape, tiling, overlap)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewWhiteSparse::render_settings()
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

  ImGui::SliderFloat("density", &this->density, 0.f, 1.f, "%.3f");
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

void ViewWhiteSparse::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("density", this->density));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewWhiteSparse::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("density", this->density));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
