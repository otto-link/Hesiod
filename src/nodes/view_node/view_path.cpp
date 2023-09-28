/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewPath::ViewPath(std::string id) : ViewNode(), hesiod::cnode::Path(id)
{
  this->set_p_control_node((gnode::Node *)this);
}

void ViewPath::render_node_specific_content()
{
  ImGui::Checkbox("Preview", &this->show_preview);

  if (this->show_preview)
    hesiod::gui::canvas_path(this->value_out, this->node_width);
}

bool ViewPath::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::Checkbox("closed", &this->closed);
  has_changed |= this->trigger_update_after_edit();

  if (hesiod::gui::canvas_path_editor(this->value_out))
    this->force_update();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
