/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewWarp::ViewWarp(std::string id) : ViewNode(), hesiod::cnode::Warp(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewWarp::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("scale", &this->scale, 0.f, 128.f, "%.1f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewWarp::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("scale", this->scale));
}

void ViewWarp::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("scale", this->scale));
}

} // namespace hesiod::vnode
