/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewGain::ViewGain(std::string id) : ViewNode(), hesiod::cnode::Gain(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view2d_port_id("output");
}

bool ViewGain::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("gain", &this->gain, 0.01f, 10.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewGain::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("gain", this->gain));
}

void ViewGain::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("gain", this->gain));
}

} // namespace hesiod::vnode
