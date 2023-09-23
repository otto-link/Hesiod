/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "gnode.hpp"
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include "hesiod/view_node.hpp"

// HELPER
static inline ImRect ImGui_GetItemRect()
{
  return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

namespace hesiod::vnode
{

ViewNode::ViewNode() : p_control_node(nullptr)
{
}

ViewNode::ViewNode(gnode::Node *p_control_node) : p_control_node(p_control_node)
{
  this->init_from_control_node();
}

gnode::Node *ViewNode::get_p_control_node()
{
  return this->p_control_node;
}

std::string ViewNode::get_preview_port_id()
{
  return this->preview_port_id;
}

void ViewNode::set_p_control_node(gnode::Node *new_p_control_node)
{
  this->p_control_node = new_p_control_node;
  this->init_from_control_node();
  LOG_DEBUG("hash_id: %d", this->p_control_node->hash_id);
}

void ViewNode::set_preview_port_id(std::string new_port_id)
{
  if (this->p_control_node->is_port_id_in_keys(new_port_id))
  {
    this->preview_port_id = new_port_id;
    this->p_control_node->update();
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

void ViewNode::init_from_control_node()
{
  // uplink connection between control and view node: when the control
  // node is updated/computed this wrapper is triggered (with the view
  // node as reference parameter) and it then triggers the
  // 'post_control_node_update' method of the node
  this->p_control_node->set_post_update_callback(
      [this](gnode::Node *p_cnode)
      { post_update_callback_wrapper(this, p_cnode); });
}

void ViewNode::post_control_node_update()
{
  LOG_DEBUG("post-update, node [%s]", this->p_control_node->id.c_str());

  if (this->preview_port_id != "")
    this->update_preview();
}

void ViewNode::render_node()
{
  ImGui::PushID((void *)this);

  std::string node_type = this->p_control_node->get_node_type();
  int         pos = this->p_control_node->get_category().find("/");
  std::string main_category = this->p_control_node->get_category().substr(0,
                                                                          pos);

  ax::NodeEditor::BeginNode(
      ax::NodeEditor::NodeId(this->p_control_node->hash_id));

  // ImGui::SetWindowFontScale(1.1f);
  if (this->p_control_node->frozen_outputs)
    ImGui::TextColored(ImColor(IM_COL32(150, 50, 50, 255)),
                       "%s",
                       node_type.c_str());
  else
    ImGui::TextUnformatted(node_type.c_str());

  ImRect text_content_rect = ImGui_GetItemRect();
  // ImGui::SetWindowFontScale(1.f);

  // ImNodes::PushColorStyle(ImNodesCol_TitleBar,
  //                         category_colors.at(main_category).base);
  // ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered,
  //                         category_colors.at(main_category).hovered);
  // ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected,
  //                         category_colors.at(main_category).selected);

  ImGui::Spacing();

  // inputs
  for (auto &[port_id, port] : this->p_control_node->get_ports())
    if (port.direction == gnode::direction::in)
    {
      ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(port.hash_id),
                               ax::NodeEditor::PinKind::Input);

      if (port.is_optional)
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f),
                           "%s",
                           port.label.c_str());
      else
        ImGui::TextUnformatted(port.label.c_str());

      ax::NodeEditor::EndPin();
    }

  // outputs
  for (auto &[port_id, port] : this->p_control_node->get_ports())
    if (port.direction == gnode::direction::out)
    {
      const char *txt = port.label.c_str();
      const float text_width = ImGui::CalcTextSize(txt).x;
      ImGui::Dummy(ImVec2(this->node_width - text_width - 8.f, 0)); // TODO
      ImGui::SameLine();

      ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(port.hash_id),
                               ax::NodeEditor::PinKind::Output);
      ImGui::TextUnformatted(txt);
      ax::NodeEditor::EndPin();
    }

  // preview
  if (this->preview_port_id != "")
  {
    if (ImGui::Checkbox("Preview", &this->show_preview))
      if (this->show_preview)
        // update only when toggle to true
        this->update_preview();

    if (this->show_preview)
    {
      ImVec2 img_size = {(float)this->shape_preview.x,
                         (float)this->shape_preview.y};

      ImGui::Image((void *)(intptr_t)this->image_texture_preview, img_size);
    }
  }

  // add content that can be overloaded and specific to the view node
  // type
  this->render_node_specific_content();

  ax::NodeEditor::EndNode();

  // title bar background
  ImRect node_content_rect = ImGui_GetItemRect();
  float  height = text_content_rect.GetBR().y - text_content_rect.GetTL().y;

  ImDrawList *draw_list = ax::NodeEditor::GetNodeBackgroundDrawList(
      this->p_control_node->hash_id);

  draw_list->AddRectFilled(ImVec2(node_content_rect.GetTL().x + 1.f,
                                  node_content_rect.GetTL().y + 1.f),
                           ImVec2(node_content_rect.GetBR().x - 1.f,
                                  text_content_rect.GetBR().y + 0.3f * height),
                           category_colors.at(main_category).hovered,
                           ax::NodeEditor::GetStyle().NodeRounding,
                           ImDrawFlags_RoundCornersTop);

  ImGui::PopID();
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

  if (ImGui::Checkbox("Auto-update", &this->p_control_node->auto_update))
    has_changed = true;

  ImGui::SameLine();

  // --- auto-update button
  bool disabled_update_button = this->p_control_node->auto_update &
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

  // --- frozen outputs button
  if (ImGui::Checkbox("Frozen outputs", &this->p_control_node->frozen_outputs))
  {
    // ignite force update when the node is unfrozzen to update
    // downstream nodes
    if (!this->p_control_node->frozen_outputs)
      this->p_control_node->force_update();
    has_changed = true;
  }

  ImGui::Separator();

  return has_changed;
}

bool ViewNode::render_settings_footer()
{
  bool has_changed = false;

  // if (this->postprocess_port_id != "")
  // {
  // }

  ImGui::Separator();

  // preview type
  if (this->preview_port_id != "" && this->show_preview)
  {
    float height = ImGui::GetStyle().ItemSpacing.y +
                   2.f * ImGui::GetTextLineHeightWithSpacing();
    if (ImGui::BeginListBox("Preview type", ImVec2(0.f, height)))
    {
      if (ImGui::Selectable("grayscale"))
        this->set_preview_type(preview_type::grayscale);
      if (ImGui::Selectable("histogram"))
        this->set_preview_type(preview_type::histogram);
      ImGui::EndListBox();
    }
  }

  ImGui::PopID();
  return has_changed;
}

void ViewNode::serialize_load(cereal::JSONInputArchive &)
{
  LOG_ERROR("serialize (input) not defined for node [%s]",
            this->p_control_node->id.c_str());
  throw std::runtime_error("undefined serialization");
}

void ViewNode::serialize_save(cereal::JSONOutputArchive &)
{
  LOG_ERROR("serialize (output) not defined for node [%s]",
            this->p_control_node->id.c_str());
  throw std::runtime_error("undefined serialization");
}

bool ViewNode::trigger_update_after_edit()
{
  if (ImGui::IsItemDeactivatedAfterEdit())
  {
    this->p_control_node->force_update();
    return true;
  }
  else
    return false;
}

void ViewNode::update_preview()
{
  if (this->preview_port_id != "" && this->show_preview)
    if (this->p_control_node->get_p_data(this->preview_port_id))
    {
      hmap::HeightMap *p_h = (hmap::HeightMap *)this->p_control_node
                                 ->get_p_data(this->preview_port_id);
      std::vector<uint8_t> img = {};

      if (this->preview_type == preview_type::grayscale)
        img = hmap::colorize_grayscale(p_h->to_array(shape_preview));
      else if (this->preview_type == preview_type::histogram)
        img = hmap::colorize_histogram(p_h->to_array(shape_preview));

      img_to_texture(img, this->shape_preview, this->image_texture_preview);
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

void img_to_texture_rgb(std::vector<uint8_t> img,
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
               GL_RGB,
               GL_UNSIGNED_BYTE,
               img.data());
}

void post_update_callback_wrapper(ViewNode *p_vnode, gnode::Node *)
{
  p_vnode->post_control_node_update();
}

} // namespace hesiod::vnode
