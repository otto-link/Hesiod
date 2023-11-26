/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewPathFinding::ViewPathFinding(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id),
      hesiod::cnode::PathFinding(id)
{
  this->set_view3d_elevation_port_id("heightmap");
  this->set_view3d_color_port_id("output");

  this->help_text =
      "Find the lowest elevation and elevation difference path for each edge "
      "of the input path, assuming this path lies on the input "
      "heightmap.\n'elevation_ratio': Balance between absolute elevation and "
      "elevation difference in the cost function.\n'distance_exponent': "
      "Exponent of the distance calculation between two points. Increasing the "
      "distance exponent of the cost function increases the cost of elevation "
      "gaps: path then tends to stay at the same elevation if possible (i.e. "
      "reduce the overall cumulative elevation gain).";
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

} // namespace hesiod::vnode
