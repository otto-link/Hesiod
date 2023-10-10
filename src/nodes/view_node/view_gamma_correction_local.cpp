/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewGammaCorrectionLocal::ViewGammaCorrectionLocal(std::string id)
    : ViewNode(), hesiod::cnode::GammaCorrectionLocal(id)
{
  LOG_DEBUG("hash_id: %d", this->hash_id);
  LOG_DEBUG("label: %s", this->label.c_str());
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewGammaCorrectionLocal::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("gamma", &this->gamma, 0.01f, 10.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("ir", &this->ir, 1, 128);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("k", &this->k, 0.001f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewGammaCorrectionLocal::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("gamma", this->gamma));
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("k", this->k));
}

void ViewGammaCorrectionLocal::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("gamma", this->gamma));
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("k", this->k));
}

} // namespace hesiod::vnode
