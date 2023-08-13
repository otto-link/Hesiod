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

  // uplink connection between control and view node: when the control
  // node is updated/computed this wrapper is triggered (with the view
  // node as reference parameter) and it then triggers the
  // 'post_control_node_update' method of the node
  this->p_control_node->set_post_update_callback(
      [this](gnode::Node *p_cnode)
      { post_update_callback_wrapper(this, p_cnode); });
}

gnode::Node *ViewNode::get_p_control_node()
{
  return this->p_control_node;
}

void ViewNode::set_preview_port_id(std::string new_port_id)
{
  if (this->p_control_node->is_port_id_in_keys(new_port_id))
  {
    this->preview_port_id = new_port_id;
    this->update_preview();
  }
  else
  {
    LOG_ERROR("port id [%s] not found", new_port_id.c_str());
    throw std::runtime_error("unknown port id");
  }
}

void ViewNode::set_preview_type(int new_preview_type)
{
  this->preview_type = new_preview_type;
  this->update_preview();
}

void ViewNode::post_control_node_update()
{
  LOG_DEBUG("post-update, node [%s]", this->id.c_str());

  if (this->preview_port_id != "")
    this->update_preview();
}

void ViewNode::render_node()
{
  std::string node_type = this->p_control_node->get_node_type();
  // ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(15, 15, 15, 255));

  // ImGui::SetWindowFontScale(0.8f);

  ImNodes::BeginNode(this->p_control_node->hash_id);
  ImNodes::BeginNodeTitleBar();
  ImGui::TextUnformatted(this->p_control_node->get_node_type().c_str());
  ImNodes::EndNodeTitleBar();

  // ImNodes::PopColorStyle();

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

  // preview
  if (this->preview_port_id != "")
  {
    ImGui::Checkbox("Preview", &this->show_preview);
    if (this->show_preview)
    {
      ImVec2 img_size = {(float)this->shape_preview.x,
                         (float)this->shape_preview.y};

      ImGui::Image((void *)(intptr_t)this->image_texture, img_size);

      if (ImGui::BeginPopupContextItem("Preview type"))
      {
        if (ImGui::Selectable("grayscale"))
          this->set_preview_type(preview_type::grayscale);
        if (ImGui::Selectable("histogram"))
          this->set_preview_type(preview_type::histogram);
        ImGui::EndPopup();
      }
    }
  }

  ImNodes::EndNode();
  ImGui::SetWindowFontScale(1.f);
}

bool ViewNode::render_settings()
{
  // just the header and the footer
  bool has_changed = false;
  has_changed |= this->render_settings_header();
  has_changed |= this->render_settings_footer();
  return has_changed;
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

void ViewNode::update_preview()
{
  if (this->preview_port_id != "")
    if (this->p_control_node->get_p_data(this->preview_port_id))
    {
      hmap::HeightMap *p_h = (hmap::HeightMap *)this->p_control_node
                                 ->get_p_data(this->preview_port_id);

      std::vector<uint8_t> img = {};

      if (this->preview_type == preview_type::grayscale)
        img = hmap::colorize_grayscale(p_h->to_array(shape_preview));
      else if (this->preview_type == preview_type::histogram)
        img = hmap::colorize_histogram(p_h->to_array(shape_preview));

      img_to_texture(img, this->shape_preview, this->image_texture);
    }
}

// HELPERS

void img_to_texture(std::vector<uint8_t> img,
                    hmap::Vec2<int>      shape,
                    GLuint              &image_texture)
{
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               shape.x,
               shape.y,
               0,
               GL_LUMINANCE,
               GL_UNSIGNED_BYTE,
               img.data());
}

void post_update_callback_wrapper(ViewNode *p_vnode, gnode::Node *p_cnode)
{
  p_vnode->post_control_node_update();
}

} // namespace hesiod::vnode
