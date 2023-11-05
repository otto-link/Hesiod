/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewLaplace::ViewLaplace(std::string id)
    : ViewNode(), hesiod::cnode::Laplace(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewLaplace::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("sigma", &this->sigma, 0.f, 0.25f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("iterations", &this->iterations, 1, 20);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewLaplace::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("sigma", this->sigma));
  ar(cereal::make_nvp("iterations", this->iterations));
}

void ViewLaplace::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("sigma", this->sigma));
  ar(cereal::make_nvp("iterations", this->iterations));
}

} // namespace hesiod::vnode
