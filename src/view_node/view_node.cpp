/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "gnode.hpp"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewNode::ViewNode(gnode::Node *p_control_node) : p_control_node(p_control_node)
{
  this->id = this->p_control_node->id;
  this->label = this->p_control_node->label;
  this->auto_update = this->p_control_node->auto_update;
}

bool ViewNode::render_settings_header()
{
  // return value, is true if anything has been changed with the GUI
  bool has_changed = false;

  // make sure the widgets are uniquely related to the current
  // object
  ImGui::PushID((void *)this);

  if (ImGui::Checkbox("Auto-update", &this->auto_update))
  {
    this->p_control_node->auto_update = this->auto_update;
    has_changed = true;
  }
  ImGui::SameLine();

  // auto-update button
  bool disabled_update_button = this->auto_update &
                                this->p_control_node->is_up_to_date;
  if (disabled_update_button)
    ImGui::BeginDisabled();

  if (ImGui::Button("Update"))
  {
    this->p_control_node->update();
    has_changed = true;
  }
  if (disabled_update_button)
    ImGui::EndDisabled();

  // state text
  ImGui::SameLine();
  if (this->p_control_node->is_up_to_date)
    ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "Up to date");
  else
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "To be updated");

  return has_changed;
}

bool ViewNode::render_settings_footer()
{
  bool has_changed = false;

  // if (this->postprocess_port_id != "")
  // {
  // }

  ImGui::PopID();
  return has_changed;
}

} // namespace hesiod::vnode
