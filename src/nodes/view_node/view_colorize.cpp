/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewColorize::ViewColorize(std::string id)
    : ViewNode(), hesiod::cnode::Colorize(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("RGB");
  this->set_view3d_elevation_port_id("input");
  this->set_view3d_color_port_id("RGB");
}

bool ViewColorize::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::Checkbox("reverse", &this->reverse);
  has_changed |= this->trigger_update_after_edit();

  ImGui::Checkbox("clamp", &this->clamp);
  has_changed |= this->trigger_update_after_edit();

  if (this->clamp)
    if (hesiod::gui::slider_vmin_vmax(this->vmin, this->vmax))
    {
      this->force_update();
      has_changed = true;
    }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewColorize::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("reverse", this->reverse));
  ar(cereal::make_nvp("clamp", this->clamp));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewColorize::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("reverse", this->reverse));
  ar(cereal::make_nvp("clamp", this->clamp));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
