/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewDigPath::ViewDigPath(std::string id)
    : ViewNode(), hesiod::cnode::DigPath(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewDigPath::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderInt("width", &this->width, 1, 32);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("decay", &this->decay, 1, 32);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("flattening_radius", &this->flattening_radius, 1, 32);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewDigPath::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("width", this->width));
  ar(cereal::make_nvp("decay", this->decay));
  ar(cereal::make_nvp("flattening_radius", this->flattening_radius));
}

void ViewDigPath::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("width", this->width));
  ar(cereal::make_nvp("decay", this->decay));
  ar(cereal::make_nvp("flattening_radius", this->flattening_radius));
}

} // namespace hesiod::vnode
