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

ViewSelectRivers::ViewSelectRivers(std::string id)
    : ViewNode(), hesiod::cnode::SelectRivers(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("input");
  this->set_view3d_color_port_id("output");
}

bool ViewSelectRivers::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("talus_ref", &this->talus_ref, 0.001f, 10.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("clipping_ratio", &this->clipping_ratio, 0.1f, 1000.f);
  has_changed |= this->trigger_update_after_edit();

  ImGui::Separator();

  if (render_settings_mask(this->smoothing,
                           this->ir_smoothing,
                           this->normalize,
                           this->inverse))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewSelectRivers::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("talus_ref", this->talus_ref));
  ar(cereal::make_nvp("clipping_ratio", this->clipping_ratio));

  ar(cereal::make_nvp("normalize", this->normalize));
  ar(cereal::make_nvp("inverse", this->inverse));
  ar(cereal::make_nvp("smoothing", this->smoothing));
  ar(cereal::make_nvp("ir_smoothing", this->ir_smoothing));
}

void ViewSelectRivers::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("talus_ref", this->talus_ref));
  ar(cereal::make_nvp("clipping_ratio", this->clipping_ratio));

  ar(cereal::make_nvp("normalize", this->normalize));
  ar(cereal::make_nvp("inverse", this->inverse));
  ar(cereal::make_nvp("smoothing", this->smoothing));
  ar(cereal::make_nvp("ir_smoothing", this->ir_smoothing));
}

} // namespace hesiod::vnode
