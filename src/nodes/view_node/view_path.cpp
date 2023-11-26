/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewPath::ViewPath(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Path(id)
{
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

  ImGui::SameLine();

  if (ImGui::Button("reverse"))
  {
    this->value_out.reverse();
    has_changed = true;
    this->force_update();
  }

  if (hesiod::gui::canvas_path_editor(this->value_out))
    this->force_update();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
