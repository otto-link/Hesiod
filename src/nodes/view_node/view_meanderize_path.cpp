/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewMeanderizePath::ViewMeanderizePath(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id),
      hesiod::cnode::MeanderizePath(id)
{
}

void ViewMeanderizePath::render_node_specific_content()
{
  ImGui::Checkbox("Preview", &this->show_preview);

  if (this->show_preview)
    hesiod::gui::canvas_path(this->value_out, this->node_width);
}

bool ViewMeanderizePath::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::SliderFloat("radius", &this->radius, 0.f, 1.f, "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("tangent_contribution",
                     &this->tangent_contribution,
                     0.f,
                     1.f,
                     "%.2f");
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderInt("iterations", &this->iterations, 1, 10);
  has_changed |= this->trigger_update_after_edit();

  ImGui::SliderFloat("transition_length_ratio",
                     &this->transition_length_ratio,
                     0.f,
                     1.f,
                     "%.2f");
  has_changed |= this->trigger_update_after_edit();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
