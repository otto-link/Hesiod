/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewZeroedEdges::ViewZeroedEdges(std::string id)
    : ViewNode(), hesiod::cnode::ZeroedEdges(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewZeroedEdges::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("sigma", &this->sigma, 0.f, 0.5f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewZeroedEdges::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("sigma", this->sigma));
}

void ViewZeroedEdges::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("sigma", this->sigma));
}

} // namespace hesiod::vnode
