/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewSteepenConvective::ViewSteepenConvective(std::string id)
    : ViewNode(), hesiod::cnode::SteepenConvective(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewSteepenConvective::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  float alpha = this->angle / 180.f * M_PI;
  ImGui::SliderAngle("angle", &alpha, -90.f, 90.f);
  this->angle = alpha / M_PI * 180.f;
  has_changed |= this->trigger_update_after_edit();

  ImGui::InputInt("iterations", &this->iterations, 1, 100);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("ir", &this->ir, 0, 128);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("dt", &this->dt, 0.f, 2.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewSteepenConvective::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("angle", this->angle));
  ar(cereal::make_nvp("iterations", this->iterations));
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("dt", this->dt));
}

void ViewSteepenConvective::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("angle", this->angle));
  ar(cereal::make_nvp("iterations", this->iterations));
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("dt", this->dt));
}

} // namespace hesiod::vnode
