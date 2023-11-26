/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewDistanceTransform::ViewDistanceTransform(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id),
      hesiod::cnode::DistanceTransform(id)
{
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");

  this->help_text =
      "Apply the distance transform (Euclidean distance) to the input "
      "heightmap. Cells with null or negative values are assumed to belong to "
      "the background.";
}

bool ViewDistanceTransform::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::Checkbox("reverse", &this->reverse);
  has_changed |= this->trigger_update_after_edit();

  if (hesiod::gui::select_shape("shape",
                                this->shape_working,
                                this->value_out.shape))
    this->force_update();

  ImGui::Separator();

  if (hesiod::gui::slider_vmin_vmax(vmin, vmax))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
