/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewNormalDisplacement::ViewNormalDisplacement(std::string id)
    : ViewNode(), hesiod::cnode::NormalDisplacement(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewNormalDisplacement::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("amount", &this->amount, 0.f, 10.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("ir", &this->ir, 0, 64);
  has_changed |= this->trigger_update_after_edit();

  ImGui::Checkbox("reverse", &this->reverse);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewNormalDisplacement::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("amount", this->amount));
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("reverse", this->reverse));
}

void ViewNormalDisplacement::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("amount", this->amount));
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("reverse", this->reverse));
}

} // namespace hesiod::vnode
