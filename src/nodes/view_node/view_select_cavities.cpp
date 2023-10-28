/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui.h"
#include "macrologger.h"

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewSelectCavities::ViewSelectCavities(std::string id)
    : ViewNode(), hesiod::cnode::SelectCavities(id)
{
  LOG_DEBUG("ViewSelectCavities::ViewSelectCavities()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewSelectCavities::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::SliderInt("int", &this->ir, 1, 256);
  has_changed |= this->trigger_update_after_edit();

  if (ImGui::Checkbox("concave", &this->concave))
    this->force_update();

  if (ImGui::Checkbox("normalize", &this->normalize))
    this->force_update();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewSelectCavities::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("concave", this->concave));
  ar(cereal::make_nvp("normalize", this->normalize));
}

void ViewSelectCavities::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("ir", this->ir));
  ar(cereal::make_nvp("concave", this->concave));
  ar(cereal::make_nvp("normalize", this->normalize));
}

} // namespace hesiod::vnode
