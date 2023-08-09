/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "gnode.hpp"
#include "imnodes.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewNode::ViewNode(gnode::Node *p_control_node) : p_control_node(p_control_node)
{
  this->id = this->p_control_node->id;
  this->label = this->p_control_node->label;
  this->auto_update = this->p_control_node->auto_update;
}

void ViewNode::render_node()
{
  ImNodes::BeginNode(this->p_control_node->hash_id);
  ImNodes::BeginNodeTitleBar();
  ImGui::TextUnformatted(this->p_control_node->label.c_str());
  ImNodes::EndNodeTitleBar();

  // inputs
  for (auto &[port_id, port] : this->p_control_node->get_ports())
    if (port.direction == gnode::direction::in)
    {
      ImNodes::BeginInputAttribute(port.hash_id);
      if (port.is_optional)
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), port.label.c_str());
      else
        ImGui::TextUnformatted(port.label.c_str());
      ImNodes::EndInputAttribute();
    }

  // outputs
  for (auto &[port_id, port] : this->p_control_node->get_ports())
    if (port.direction == gnode::direction::out)
    {
      const char *txt = port.label.c_str();
      ImNodes::BeginOutputAttribute(port.hash_id);
      const float text_width = ImGui::CalcTextSize(txt).x;
      ImGui::Indent(this->node_width - text_width);
      ImGui::TextUnformatted(txt);
      ImNodes::EndOutputAttribute();
    }

  // // preview
  // ImGui::Checkbox("Preview", &this->show_preview);
  // if (this->show_preview)
  // {
  //   if (preview_port_id != "")
  //     this->update_preview(); // TODO optimize: update only when
  //                             // underlying data are modified

  //   ImVec2 img_size = {(float)shape_preview.x, (float)shape_preview.y};
  //   ImGui::Image((void *)(intptr_t)image_texture, img_size);

  //   if (ImGui::BeginPopupContextItem("Preview type"))
  //   {
  //     if (ImGui::Selectable("grayscale"))
  //       this->preview_type = preview_type::grayscale;
  //     if (ImGui::Selectable("histogram"))
  //       this->preview_type = preview_type::histogram;
  //     ImGui::EndPopup();
  //   }
  // }

  ImNodes::EndNode();
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
