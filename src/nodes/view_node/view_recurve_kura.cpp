/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewRecurveKura::ViewRecurveKura(std::string id)
    : ViewNode(), hesiod::cnode::RecurveKura(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewRecurveKura::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("a", &this->a, 0.01f, 4.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("b", &this->b, 0.01f, 4.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewRecurveKura::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("a", this->a));
  ar(cereal::make_nvp("b", this->b));
}

void ViewRecurveKura::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("a", this->a));
  ar(cereal::make_nvp("b", this->b));
}

} // namespace hesiod::vnode
