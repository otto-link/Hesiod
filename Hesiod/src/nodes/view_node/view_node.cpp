/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <list>

#include "gnode.hpp"
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include "hesiod/view_node.hpp"
#include "hesiod/widgets.hpp"

// --- HELPERS
static inline ImRect ImGui_GetItemRect()
{
  return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

int count_leading_character(const std::string &str, char target)
{
  int count = 0;

  for (char ch : str)
  {
    if (ch == target)
      count++;
    else
      break;
  }
  return count;
}

// --- Hesiod

namespace hesiod::vnode
{

ViewNode::ViewNode(std::string id) : hesiod::cnode::ControlNode(id)
{
  // setup callbacks
  this->set_pre_update_callback([this](gnode::Node *)
                                { this->pre_control_node_update(); });
  this->set_post_update_callback([this](gnode::Node *)
                                 { this->post_control_node_update(); });
}

std::string ViewNode::get_preview_port_id()
{
  return this->preview_port_id;
}

std::string ViewNode::get_view3d_elevation_port_id()
{
  return this->view3d_elevation_port_id;
}

std::string ViewNode::get_view3d_color_port_id()
{
  return this->view3d_color_port_id;
}

void ViewNode::set_preview_port_id(std::string new_port_id)
{
  if (this->is_port_id_in_keys(new_port_id))
  {
    this->preview_port_id = new_port_id;
    this->update();
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

void ViewNode::set_view3d_elevation_port_id(std::string new_port_id)
{
  if (this->is_port_id_in_keys(new_port_id))
    // NB - viewer update is carried out by the ViewTree object (above)
    this->view3d_elevation_port_id = new_port_id;
  else
  {
    LOG_ERROR("port id [%s] not found", new_port_id.c_str());
    throw std::runtime_error("unknown port id");
  }
}

void ViewNode::set_view3d_color_port_id(std::string new_port_id)
{
  if (this->is_port_id_in_keys(new_port_id))
    this->view3d_color_port_id = new_port_id;
  else
  {
    LOG_ERROR("port id [%s] not found", new_port_id.c_str());
    throw std::runtime_error("unknown port id");
  }
}

void ViewNode::pre_control_node_update()
{
  LOG_DEBUG("pre-update, node [%s]", this->id.c_str());

  this->timer.reset();
}

void ViewNode::post_control_node_update()
{
  LOG_DEBUG("post-update, node [%s]", this->id.c_str());

  this->update_time = this->timer.stop();

  if (this->preview_port_id != "")
    this->update_preview();
}

bool ViewNode::render_attribute_settings()
{
  bool has_changed = false;

  std::list<std::string> attr_key_queue = {};
  for (auto &[k, v] : this->attr)
    attr_key_queue.push_back(k);

  // start rendering the parameters using the ordered list
  for (auto &k : this->attr_ordered_key)
  {
    // indentation
    int         indent = count_leading_character(k, '_');
    std::string key = k.substr(indent, k.size() - indent);

    for (int i = 0; i < indent; i++)
      ImGui::Indent();

    // verity the key indeed exist before using it
    if (!this->attr.contains(key))
    {
      LOG_ERROR("unknown attribute id [%s], check the attr_ordered_key "
                "definition for node [%s]",
                key.c_str(),
                this->id.c_str());
      throw std::runtime_error("unknown attribute id");
    }

    has_changed |= hesiod::gui::render_attribute_widget(this->attr.at(key),
                                                        key);

    for (int i = 0; i < indent; i++)
      ImGui::Unindent();

    attr_key_queue.remove(key);
  }

  // render the remaining parameters not present in the ordered list
  for (auto &k : attr_key_queue)
    has_changed |= hesiod::gui::render_attribute_widget(this->attr.at(k), k);

  return has_changed;
}

void ViewNode::render_node()
{
  ImGui::PushID((void *)this);

  std::string node_type = this->get_node_type();
  int         pos = this->get_category().find("/");
  std::string main_category = this->get_category().substr(0, pos);

  std::string node_label = node_type;
  {
    // truncate node id to make it fit within the node width
    float char_width = ImGui::CalcTextSize("a").x;
    int   nchar = (int)(this->node_width / char_width);
    if (nchar < (int)node_label.size())
    {
      nchar -= 3;
      node_label = node_label.substr(0, nchar) + "...";
    }
  }

  ax::NodeEditor::BeginNode(ax::NodeEditor::NodeId(this->hash_id));

  if (this->frozen_outputs)
    ImGui::TextColored(ImColor(IM_COL32(150, 50, 50, 255)),
                       "%s",
                       node_label.c_str());
  else
    ImGui::TextUnformatted(node_label.c_str());

  ImRect text_content_rect = ImGui_GetItemRect();

  ImGui::Spacing();

  // inputs
  for (auto &[port_id, port] : this->get_ports())
    if (port.direction == gnode::direction::in)
    {
      ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(port.hash_id),
                               ax::NodeEditor::PinKind::Input);

      ImU32 color = dtype_colors.at(port.dtype).hovered;
      if (port.is_optional)
        color = dtype_colors.at(port.dtype).base;

      hesiod::gui::draw_icon(hesiod::gui::square,
                             {10.f, 10.f},
                             color,
                             port.is_connected);

      ax::NodeEditor::EndPin();
      ImGui::SameLine();

      if (port.is_optional)
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f),
                           "%s",
                           port.label.c_str());
      else
        ImGui::TextUnformatted(port.label.c_str());
    }

  // outputs
  for (auto &[port_id, port] : this->get_ports())
    if (port.direction == gnode::direction::out)
    {
      const char *txt = port.label.c_str();
      const float text_width = ImGui::CalcTextSize(txt).x;
      ImGui::Dummy(
          ImVec2(this->node_width - text_width - 8.f - 18.f, 0)); // TODO
      ImGui::SameLine();

      ImGui::TextUnformatted(txt);
      ImGui::SameLine();

      ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(port.hash_id),
                               ax::NodeEditor::PinKind::Output);
      hesiod::gui::draw_icon(hesiod::gui::square,
                             {10.f, 10.f},
                             dtype_colors.at(port.dtype).hovered,
                             port.is_connected);
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
      this->hash_id);

  draw_list->AddRectFilled(ImVec2(node_content_rect.GetTL().x + 1.f,
                                  node_content_rect.GetTL().y + 1.f),
                           ImVec2(node_content_rect.GetBR().x - 1.f,
                                  text_content_rect.GetBR().y + 0.3f * height),
                           category_colors.at(main_category).hovered,
                           ax::NodeEditor::GetStyle().NodeRounding,
                           ImDrawFlags_RoundCornersTop);

  ImGui::PopID();
}

void ViewNode::render_node_minimalist()
{
  ImGui::PushID((void *)this);

  std::string node_type = this->get_node_type();
  int         pos = this->get_category().find("/");
  std::string main_category = this->get_category().substr(0, pos);

  std::string node_label = node_type;
  {
    // truncate node id to make it fit within the node width
    float char_width = ImGui::CalcTextSize("a").x;
    int   nchar = (int)(this->node_width / char_width);
    if (nchar < (int)node_label.size())
    {
      nchar -= 3;
      node_label = node_label.substr(0, nchar) + "...";
    }
  }

  ax::NodeEditor::BeginNode(ax::NodeEditor::NodeId(this->hash_id));

  if (this->frozen_outputs)
    ImGui::TextColored(ImColor(IM_COL32(150, 50, 50, 255)),
                       "%s",
                       node_label.c_str());
  else
    ImGui::TextColored(ImColor(category_colors.at(main_category).hovered),
                       "%s",
                       node_label.c_str());

  ImGui::Spacing();

  // inputs
  for (auto &[port_id, port] : this->get_ports())
    if (port.direction == gnode::direction::in)
    {
      ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(port.hash_id),
                               ax::NodeEditor::PinKind::Input);

      ImU32 color = dtype_colors.at(port.dtype).hovered;
      if (port.is_optional)
        color = dtype_colors.at(port.dtype).base;

      hesiod::gui::draw_icon(hesiod::gui::square,
                             {10.f, 10.f},
                             color,
                             port.is_connected);

      ax::NodeEditor::EndPin();
      ImGui::SameLine();

      if (port.is_optional)
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f),
                           "%s",
                           port.label.c_str());
      else
        ImGui::TextUnformatted(port.label.c_str());
    }

  // outputs
  for (auto &[port_id, port] : this->get_ports())
    if (port.direction == gnode::direction::out)
    {
      const char *txt = port.label.c_str();
      const float text_width = ImGui::CalcTextSize(txt).x;
      ImGui::Dummy(
          ImVec2(this->node_width - text_width - 8.f - 18.f, 0)); // TODO
      ImGui::SameLine();

      ImGui::TextUnformatted(txt);
      ImGui::SameLine();

      ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(port.hash_id),
                               ax::NodeEditor::PinKind::Output);
      hesiod::gui::draw_icon(hesiod::gui::square,
                             {10.f, 10.f},
                             dtype_colors.at(port.dtype).hovered,
                             port.is_connected);
      ax::NodeEditor::EndPin();
    }

  ax::NodeEditor::EndNode();

  ImGui::PopID();
}

bool ViewNode::render_settings()
{
  // just the header and the footer
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  // automatic layout
  ImGui::SeparatorText("Settings");
  has_changed |= this->render_attribute_settings();

  if (has_changed)
    this->force_update();

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

  // --- buttons
  ImGui::Checkbox("Auto-update", &this->auto_update);

  // frozen outputs button
  ImGui::SameLine();
  if (ImGui::Checkbox("Frozen outputs", &this->frozen_outputs))
  {
    // ignite force update when the node is unfrozzen to update
    // downstream nodes
    if (!this->frozen_outputs)
      this->force_update();
    has_changed = true;
  }

  // help
  ImGui::SameLine();
  ImGui::Checkbox("Show help", &this->show_help);

  // auto-update button
  bool disabled_update_button = this->auto_update & this->is_up_to_date;
  if (disabled_update_button)
    ImGui::BeginDisabled();

  if (ImGui::Button("Update"))
  {
    // switch auto_update state to true to allow node update, if not
    // update will be blocked in GNode update procedure
    this->auto_update = true;
    this->force_update();
    this->auto_update = false;
  }
  if (disabled_update_button)
    ImGui::EndDisabled();

  // state text
  ImGui::SameLine();
  if (this->is_up_to_date)
    ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "Up to date");
  else
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "To be updated");
  ImGui::SameLine();
  ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "(%.2f ms)", this->update_time);

  return has_changed;
}

bool ViewNode::render_settings_footer()
{
  bool has_changed = false;

  // preview type
  if (this->preview_port_id != "" && this->show_preview)
  {
    ImGui::SeparatorText("Preview");
    float height = ImGui::GetStyle().ItemSpacing.y +
                   3.f * ImGui::GetTextLineHeightWithSpacing();
    if (ImGui::BeginListBox("Preview type", ImVec2(0.f, height)))
    {
      if (ImGui::Selectable("grayscale"))
        this->set_preview_type(preview_type::grayscale);
      else if (ImGui::Selectable("jet"))
        this->set_preview_type(preview_type::jet);
      else if (ImGui::Selectable("histogram"))
        this->set_preview_type(preview_type::histogram);
      ImGui::EndListBox();
    }
  }

  // help text
  if (this->show_help)
  {
    ImGui::SeparatorText("Help");
    ImGui::PushTextWrapPos(0.0f);
    ImGui::TextUnformatted((char *)this->help_text.c_str());
    ImGui::PopTextWrapPos();
  }

  ImGui::PopID();
  return has_changed;
}

bool ViewNode::trigger_update_after_edit()
{
  if (ImGui::IsItemDeactivatedAfterEdit())
  {
    this->force_update();
    return true;
  }
  else
    return false;
}

void ViewNode::update_preview()
{
  if (this->preview_port_id != "" && this->show_preview)
  {
    void *p_data = this->get_p_data(this->preview_port_id);
    if (p_data)
    {
      int port_dtype = this->get_port_ref_by_id(preview_port_id)->dtype;

      switch (port_dtype)
      {

      case hesiod::cnode::dArray:
      {
        hmap::Array         *p_a = (hmap::Array *)p_data;
        std::vector<uint8_t> img = {};

        hmap::Array array = p_a->resample_to_shape(this->shape_preview);

        if (this->preview_type == preview_type::grayscale)
        {
          img = hmap::colorize_grayscale(array);
          img_to_texture(img, this->shape_preview, this->image_texture_preview);
        }
        else if (this->preview_type == preview_type::jet)
        {
          img = hmap::colorize(array,
                               array.min(),
                               array.max(),
                               hmap::cmap::jet,
                               false);
          img_to_texture_rgb(img,
                             this->shape_preview,
                             this->image_texture_preview);
        }
        else if (this->preview_type == preview_type::histogram)
        {
          img = hmap::colorize_histogram(array);
          img_to_texture(img, this->shape_preview, this->image_texture_preview);
        }
      }
      break;

      case hesiod::cnode::dCloud:
      {
        hmap::Cloud          cloud = *(hmap::Cloud *)p_data;
        std::vector<uint8_t> img = {};

        hmap::Array       array = hmap::Array(this->shape_preview);
        hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);

        if (cloud.get_npoints() > 0)
        {
          cloud.set_values(1.f);
          cloud.to_array(array, bbox);
        }

        img = hmap::colorize_grayscale(array);
        img_to_texture(img, this->shape_preview, this->image_texture_preview);
      }
      break;

      case hesiod::cnode::dHeightMap:
      {
        hmap::HeightMap     *p_h = (hmap::HeightMap *)p_data;
        std::vector<uint8_t> img = {};

        hmap::Array array = p_h->to_array(this->shape_preview);

        if (this->preview_type == preview_type::grayscale)
        {
          img = hmap::colorize_grayscale(array);
          img_to_texture(img, this->shape_preview, this->image_texture_preview);
        }
        else if (this->preview_type == preview_type::jet)
        {
          img = hmap::colorize(array,
                               array.min(),
                               array.max(),
                               hmap::cmap::jet,
                               false);
          img_to_texture_rgb(img,
                             this->shape_preview,
                             this->image_texture_preview);
        }
        else if (this->preview_type == preview_type::histogram)
        {
          img = hmap::colorize_histogram(array);
          img_to_texture(img, this->shape_preview, this->image_texture_preview);
        }
      }
      break;

      case hesiod::cnode::dHeightMapRGB:
      {
        hmap::HeightMapRGB  *p_c = (hmap::HeightMapRGB *)p_data;
        std::vector<uint8_t> img = {};

        if (p_c->shape.x > 0)
          img = p_c->to_img_8bit(this->shape_preview);

        img_to_texture_rgb(img,
                           this->shape_preview,
                           this->image_texture_preview);
      }
      break;

      case hesiod::cnode::dPath:
      {
        hmap::Path           path = *(hmap::Path *)p_data;
        std::vector<uint8_t> img = {};

        hmap::Array       array = hmap::Array(this->shape_preview);
        hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);

        if (path.get_npoints() > 1)
        {
          path.set_values(1.f);
          path.to_array(array, bbox);
        }

        img = hmap::colorize_grayscale(array);
        img_to_texture(img, this->shape_preview, this->image_texture_preview);
      }
      break;
      }
    }
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

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

} // namespace hesiod::vnode
