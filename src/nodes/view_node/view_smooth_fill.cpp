/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewSmoothFill::ViewSmoothFill(std::string id)
    : ViewNode(), hesiod::cnode::SmoothFill(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewSmoothFill::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderInt("int", &this->ir, 1, 256);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("k", &this->k, 0.001f, 1.f, "%.3f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewSmoothFill::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("k", this->k));
}

void ViewSmoothFill::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("k", this->k));
}

} // namespace hesiod::vnode
