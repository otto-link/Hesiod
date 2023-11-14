/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewCombineMask::ViewCombineMask(std::string id)
    : ViewNode(), hesiod::cnode::CombineMask(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_color_port_id("output");
}

bool ViewCombineMask::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  if (ImGui::RadioButton("Union", &this->method, 0) ||
      ImGui::RadioButton("Intersection", &this->method, 1) ||
      ImGui::RadioButton("Exclusion", &this->method, 2))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewCombineMask::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("method", this->method));
}

void ViewCombineMask::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("method", this->method));
}

} // namespace hesiod::vnode
