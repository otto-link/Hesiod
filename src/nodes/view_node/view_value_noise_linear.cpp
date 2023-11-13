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

ViewValueNoiseLinear::ViewValueNoiseLinear(std::string     id,
                                           hmap::Vec2<int> shape,
                                           hmap::Vec2<int> tiling,
                                           float           overlap)
    : ViewNode(), hesiod::cnode::ValueNoiseLinear(id, shape, tiling, overlap)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewValueNoiseLinear::render_settings()
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

  if (hesiod::gui::drag_float_vec2(this->kw, this->link_kxy))
  {
    has_changed = true;
    this->force_update();
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

void ViewValueNoiseLinear::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("kw.x", this->kw.x));
  ar(cereal::make_nvp("kw.y", this->kw.y));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewValueNoiseLinear::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("kw.x", this->kw.x));
  ar(cereal::make_nvp("kw.y", this->kw.y));
  ar(cereal::make_nvp("seed", this->seed));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
