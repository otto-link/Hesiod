/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewConvolveSVD::ViewConvolveSVD(std::string id)
    : ViewNode(), hesiod::cnode::ConvolveSVD(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewConvolveSVD::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderInt("rank", &this->rank, 1, 8);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewConvolveSVD::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("rank", this->rank));
}

void ViewConvolveSVD::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("rank", this->rank));
}

} // namespace hesiod::vnode
