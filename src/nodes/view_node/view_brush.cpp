/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui.h"
#include "macrologger.h"
#include <cereal/archives/json.hpp>

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewBrush::ViewBrush(std::string     id,
                     hmap::Vec2<int> shape,
                     hmap::Vec2<int> tiling,
                     float           overlap)
    : ViewNode(), hesiod::cnode::Brush(id, shape, tiling, overlap)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewBrush::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  if (hesiod::gui::hmap_brush_editor(this->value_out))
    this->force_update();

  if (this->remap)
  {
    ImGui::Checkbox("inverse", &this->inverse);
    has_changed |= this->trigger_update_after_edit();

    if (hesiod::gui::slider_vmin_vmax(vmin, vmax))
    {
      this->force_update();
      has_changed = true;
    }
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewBrush::serialize_save(cereal::JSONOutputArchive &ar)
{
  // TODO SAVE HEIGHTMAP DATA
  LOG_ERROR("!!!heightmap data are not saved!!!");
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
  ar(cereal::make_nvp("inverse", this->inverse));
}

void ViewBrush::serialize_load(cereal::JSONInputArchive &ar)
{
  LOG_ERROR("!!!heightmap data are not saved!!!");
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
  ar(cereal::make_nvp("inverse", this->inverse));
}

} // namespace hesiod::vnode
