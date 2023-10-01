/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewBlend::ViewBlend(std::string id) : ViewNode(), hesiod::cnode::Blend(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewBlend::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  if (hesiod::gui::listbox_map_enum(this->blending_method_map,
                                    this->method,
                                    128.f))
  {
    this->force_update();
    has_changed = true;
  }

  if (this->method == hesiod::cnode::blending_method::maximum_smooth ||
      this->method == hesiod::cnode::blending_method::minimum_smooth)
  {
    ImGui::SliderFloat("k", &this->k, 0.01f, 1.f, "%.2f");
    has_changed |= this->trigger_update_after_edit();
  }

  if (this->method == hesiod::cnode::blending_method::gradients)
  {
    ImGui::SliderInt("ir", &this->ir, 1, 128);
    has_changed |= this->trigger_update_after_edit();
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewBlend::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("method", this->method));
  ar(cereal::make_nvp("k", this->k));
  ar(cereal::make_nvp("ir", this->ir));
}

void ViewBlend::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("method", this->method));
  ar(cereal::make_nvp("k", this->k));
  ar(cereal::make_nvp("ir", this->ir));
}

} // namespace hesiod::vnode
