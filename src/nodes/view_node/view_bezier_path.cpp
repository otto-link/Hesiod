/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewBezierPath::ViewBezierPath(std::string id)
    : ViewNode(), hesiod::cnode::BezierPath(id)
{
  this->set_p_control_node((gnode::Node *)this);
}

void ViewBezierPath::render_node_specific_content()
{
  ImGui::Checkbox("Preview", &this->show_preview);

  if (this->show_preview)
    hesiod::gui::canvas_path(this->value_out, this->node_width);
}

bool ViewBezierPath::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("curvature_ratio",
                     &this->curvature_ratio,
                     0.f,
                     1.f,
                     "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("edge_divisions", &this->edge_divisions, 1, 32);
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewBezierPath::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("curvature_ratio", this->curvature_ratio));
  ar(cereal::make_nvp("edge_divisions", this->edge_divisions));
}

void ViewBezierPath::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("curvature_ratio", this->curvature_ratio));
  ar(cereal::make_nvp("edge_divisions", this->edge_divisions));
}

} // namespace hesiod::vnode
