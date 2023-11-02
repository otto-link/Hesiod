/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewPathFinding::ViewPathFinding(std::string id)
    : ViewNode(), hesiod::cnode::PathFinding(id)
{
  this->set_p_control_node((gnode::Node *)this);
}

void ViewPathFinding::render_node_specific_content()
{
  ImGui::Checkbox("Preview", &this->show_preview);

  if (this->show_preview)
    hesiod::gui::canvas_path(this->value_out, this->node_width);
}

bool ViewPathFinding::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("elevation_ratio",
                     &this->elevation_ratio,
                     0.f,
                     0.9f,
                     "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("distance_exponent",
                     &this->distance_exponent,
                     0.5f,
                     1.5f,
                     "%.1f");
  has_changed |= this->trigger_update_after_edit();

  // TODO - retrieve max shape from input
  if (hesiod::gui::select_shape("shape",
                                this->wshape,
                                hmap::Vec2<int>(1024, 1024)))
    this->force_update();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewPathFinding::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("wshape.x", this->wshape.x));
  ar(cereal::make_nvp("wshape.y", this->wshape.y));
  ar(cereal::make_nvp("elevation_ratio", this->elevation_ratio));
  ar(cereal::make_nvp("distance_exponent", this->distance_exponent));
}

void ViewPathFinding::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("wshape.x", this->wshape.x));
  ar(cereal::make_nvp("wshape.y", this->wshape.y));
  ar(cereal::make_nvp("elevation_ratio", this->elevation_ratio));
  ar(cereal::make_nvp("distance_exponent", this->distance_exponent));
}

} // namespace hesiod::vnode
