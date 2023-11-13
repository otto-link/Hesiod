/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewPathToHeightmap::ViewPathToHeightmap(std::string     id,
                                         hmap::Vec2<int> shape,
                                         hmap::Vec2<int> tiling,
                                         float           overlap)
    : ViewNode(), hesiod::cnode::PathToHeightmap(id, shape, tiling, overlap)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
  this->set_view3d_color_port_id("path");
}

bool ViewPathToHeightmap::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::Checkbox("filled", &this->filled);
  has_changed |= this->trigger_update_after_edit();

  if (hesiod::gui::slider_vmin_vmax(this->vmin, this->vmax))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewPathToHeightmap::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("filled", this->filled));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewPathToHeightmap::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("filled", this->filled));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
