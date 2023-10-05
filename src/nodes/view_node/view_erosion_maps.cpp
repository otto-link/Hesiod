/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewErosionMaps::ViewErosionMaps(std::string id)
    : ViewNode(), hesiod::cnode::ErosionMaps(id)
{
  this->set_p_control_node((gnode::Node *)this);
}

bool ViewErosionMaps::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("tolerance", &this->tolerance, 0.f, 0.5f, "%.3f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewErosionMaps::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("tolerance", this->tolerance));
}

void ViewErosionMaps::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("tolerance", this->tolerance));
}
  
} // namespace hesiod::vnode
