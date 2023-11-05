/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewDepressionFilling::ViewDepressionFilling(std::string id)
    : ViewNode(), hesiod::cnode::DepressionFilling(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewDepressionFilling::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("epsilon", &this->epsilon, 0.0001f, 0.001f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::InputInt("iterations", &this->iterations);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewDepressionFilling::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("iterations", this->iterations));
  ar(cereal::make_nvp("epsilon", this->epsilon));
}

void ViewDepressionFilling::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("iterations", this->iterations));
  ar(cereal::make_nvp("epsilon", this->epsilon));
}

} // namespace hesiod::vnode
