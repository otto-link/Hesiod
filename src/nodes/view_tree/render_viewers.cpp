/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
#include "imgui_internal.h"
#include "macrologger.h"
#include <imgui_node_editor.h>

#include "hesiod/viewer.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

void ViewTree::render_view2d()
{
  ImGui::PushID((void *)this);

  ImGui::Text("%s", this->viewer_node_id.c_str());

  hesiod::vnode::ViewControlNode *p_vnode =
      this->get_view_control_node_ref_by_id(this->viewer_node_id);

  if (p_vnode->get_preview_port_id() != "")
  {
    if (ImGui::Button("Reset view"))
    {
      this->view2d_zoom = 100.f;
      this->view2d_uv0 = {0.f, 0.f};
    }
    ImGui::SameLine();

    if (hesiod::gui::listbox_map_enum(this->cmap_map, this->cmap_view2d, 128.f))
      this->update_image_texture_view2d();

    ImGui::SameLine();
    if (ImGui::Checkbox("Hillshading", &this->hillshade_view2d))
      this->update_image_texture_view2d();

    {
      bool has_changed = false;
      if (ImGui::Button("256 x 256"))
      {
        this->shape_view2d = {256, 256};
        has_changed = true;
      }
      ImGui::SameLine();
      if (ImGui::Button("512 x 512"))
      {
        this->shape_view2d = {512, 512};
        has_changed = true;
      }
      ImGui::SameLine();
      if (ImGui::Button("1024 x 1024"))
      {
        this->shape_view2d = {1024, 1024};
        has_changed = true;
      }

      if (has_changed)
        this->update_image_texture_view2d();
    }

    float  window_width = ImGui::GetContentRegionAvail().x;
    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImVec2 uv1 = {this->view2d_uv0[0] + 100.f / this->view2d_zoom,
                  this->view2d_uv0[1] + 100.f / this->view2d_zoom};

    ImVec2 p0 = ImVec2(pos.x, pos.y);
    ImVec2 p1 = ImVec2(pos.x + window_width, pos.y + window_width);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(p0, p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddImage((void *)(intptr_t)this->image_texture_view2d,
                        p0,
                        p1,
                        this->view2d_uv0,
                        uv1);
    draw_list->AddRect(p0, p1, IM_COL32(255, 255, 255, 255));

    ImGui::InvisibleButton("##image2d", ImVec2(window_width, window_width));

    // zooming and panning
    ImGuiIO &io = ImGui::GetIO();
    {

      ImGui::SetItemKeyOwner(ImGuiKey_MouseLeft);
      ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);
      if (ImGui::IsItemHovered())
      {
        // zoom
        if (io.MouseWheel)
          this->view2d_zoom = std::max(1.f,
                                       this->view2d_zoom *
                                           (1.f + 0.05f * io.MouseWheel));

        // position
        if (ImGui::IsMouseDown(2))
        {
          ImVec2 window_size = ImGui::GetWindowSize();
          float  du = -io.MouseDelta.x / window_size[0];
          float  dv = -io.MouseDelta.y / window_size[1];
          this->view2d_uv0 = {this->view2d_uv0[0] + du,
                              this->view2d_uv0[1] + dv};
        }
      }
    }
  }

  ImGui::PopID();
}

void ViewTree::render_view3d()
{
  ImGui::PushID((void *)this);

  ImGui::Text("%s", this->viewer_node_id.c_str());

  hesiod::vnode::ViewControlNode *p_vnode =
      this->get_view_control_node_ref_by_id(this->viewer_node_id);

  if (p_vnode->get_preview_port_id() != "")
  {
    // --- controls
    {
      bool has_changed = false;
      if (ImGui::Button("256 x 256"))
      {
        this->shape_view3d = {256, 256};
        has_changed = true;
      }
      ImGui::SameLine();
      if (ImGui::Button("512 x 512"))
      {
        this->shape_view3d = {512, 512};
        has_changed = true;
      }
      ImGui::SameLine();
      if (ImGui::Button("1024 x 1024"))
      {
        this->shape_view3d = {1024, 1024};
        has_changed = true;
      }

      if (has_changed)
      {
        this->update_view3d_basemesh();
        this->update_image_texture_view3d();
      }
    }

    {
      if (ImGui::Button("Top"))
      {
        alpha_x = 90.f;
        alpha_y = 0.f;
        delta_x = 0.f;
        delta_y = 0.f;
        scale = 1.f;
        this->update_image_texture_view3d(true);
      }
      ImGui::SameLine();

      if (ImGui::Checkbox("Wireframe", &this->wireframe))
        this->update_image_texture_view3d(true);
      ImGui::SameLine();

      ImGui::Checkbox("Auto rotate", &this->auto_rotate);
      if (this->auto_rotate)
        this->update_image_texture_view3d(true);
      ImGui::SameLine();

      if (ImGui::SliderFloat("h_scale", &this->h_scale, 0.f, 2.f, "%.2f"))
        this->update_image_texture_view3d(true);
    }

    // --- 3D rendering viewport
    float window_width = ImGui::GetContentRegionAvail().x;
    {
      ImVec2 pos = ImGui::GetCursorScreenPos();

      ImVec2 p0 = ImVec2(pos.x, pos.y);
      ImVec2 p1 = ImVec2(pos.x + window_width, pos.y + window_width);

      ImDrawList *draw_list = ImGui::GetWindowDrawList();

      draw_list->AddImage((void *)(intptr_t)this->image_texture_view3d,
                          p0,
                          p1,
                          ImVec2(0, 1),
                          ImVec2(1, 0));
      draw_list->AddRect(p0, p1, IM_COL32(255, 255, 255, 255));
      ImGui::InvisibleButton("##image3d", ImVec2(window_width, window_width));
    }

    ImGuiIO &io = ImGui::GetIO();
    {
      ImGui::SetItemKeyOwner(ImGuiKey_MouseLeft);
      ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);
      if (ImGui::IsItemHovered())
      {
        float factor = 1.f;
        if (io.KeyMods == ImGuiMod_Shift)
          factor = 0.1f;

        ImGuiIO &io = ImGui::GetIO();
        if (io.MouseWheel != 0.f)
        {
          float dscale = factor * this->scale * io.MouseWheel * 0.1f;
          this->scale = std::max(0.05f, this->scale + dscale);
          this->update_image_texture_view3d(true);
        }

        if (ImGui::IsMouseDown(0))
        {
          this->alpha_y += 100.f * factor * io.MouseDelta.x / window_width;
          this->alpha_x += 100.f * factor * io.MouseDelta.y / window_width;
          this->update_image_texture_view3d(true);
        }
        if (ImGui::IsMouseDown(2))
        {
          this->delta_x += 4.f * factor * io.MouseDelta.x / window_width;
          this->delta_y -= 4.f * factor * io.MouseDelta.y / window_width;
          this->update_image_texture_view3d(true);
        }
      }
    }
  }

  ImGui::PopID();
}

void ViewTree::render_settings(std::string node_id)
{
  this->get_view_control_node_ref_by_id(node_id)->render_settings();
}

void ViewTree::update_image_texture_view2d()
{
  if (this->is_node_id_in_keys(this->viewer_node_id))
  {
    hesiod::vnode::ViewControlNode *p_vnode =
        this->get_view_control_node_ref_by_id(this->viewer_node_id);

    if (p_vnode->get_preview_port_id() != "")
    {
      void *p_data = p_vnode->get_p_data(p_vnode->get_preview_port_id());

      if (p_data)
      {
        hmap::HeightMap *p_h = (hmap::HeightMap *)p_data;
        hmap::Array      array = p_h->to_array(this->shape_view2d);

        std::vector<uint8_t> img = hmap::colorize(array,
                                                  array.min(),
                                                  array.max(),
                                                  this->cmap_view2d,
                                                  this->hillshade_view2d);

        img_to_texture_rgb(img, this->shape_view2d, this->image_texture_view2d);
      }
    }
  }
}

void ViewTree::update_image_texture_view3d(bool only_matrix_update)
{
  if (this->is_node_id_in_keys(this->viewer_node_id))
  {
    hesiod::vnode::ViewControlNode *p_vnode =
        this->get_view_control_node_ref_by_id(this->viewer_node_id);

    if (p_vnode->get_preview_port_id() != "")
    {
      void *p_data = p_vnode->get_p_data(p_vnode->get_preview_port_id());

      if (p_data)
      {
        if (!only_matrix_update)
        {
          hmap::HeightMap *p_h = (hmap::HeightMap *)p_data;
          hmap::Array      array = p_h->to_array(this->shape_view3d);

          hesiod::viewer::update_vertex_elevations(array,
                                                   this->vertices,
                                                   this->colors);

          glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
          glBufferData(GL_ARRAY_BUFFER,
                       sizeof(this->vertices[0]) * this->vertices.size(),
                       (GLvoid *)&this->vertices[0],
                       GL_STATIC_DRAW);

          glBindBuffer(GL_ARRAY_BUFFER, this->color_buffer);
          glBufferData(GL_ARRAY_BUFFER,
                       sizeof(this->colors[0]) * this->colors.size(),
                       (GLvoid *)&this->colors[0],
                       GL_STATIC_DRAW);
        }

        hesiod::viewer::bind_framebuffer(this->FBO);

        glUseProgram(this->shader_id);
        glClearColor(0.1f, 0.1f, 0.1f, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_FRONT);

        // reset glViewport and scale framebuffer
        glViewport(0, 0, this->shape_view3d.x, this->shape_view3d.y);

        if (this->wireframe)
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        // colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, this->color_buffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        glm::mat4 combined_matrix;
        {
          if (this->auto_rotate)
            this->alpha_y += 0.1f;

          glm::mat4 scale_matrix = glm::scale(
              glm::mat4(1.0f),
              glm::vec3(this->scale, this->scale * this->h_scale, this->scale));

          glm::mat4 rotation_matrix_y = glm::rotate(
              glm::mat4(1.0f),
              glm::radians(this->alpha_y),
              glm::vec3(0.0f, 1.0f, 0.0f));

          glm::mat4 rotation_matrix_x = glm::rotate(
              glm::mat4(1.0f),
              glm::radians(this->alpha_x),
              glm::vec3(1.0f, 0.0f, 0.0f));

          glm::mat4 rotation_matrix = rotation_matrix_x * rotation_matrix_y;

          glm::mat4 transalation_matrix = glm::translate(
              glm::mat4(1.f),
              glm::vec3(this->delta_x, this->delta_y, 0.f));

          glm::mat4 view_matrix = glm::translate(glm::mat4(1.f),
                                                 glm::vec3(0.f, 0.f, -2.f));

          // Define perspective projection parameters
          float     fov = 60.0f;        // Field of view in degrees
          float     aspect_ratio = 1.;  // Aspect ratio (width/height)
          float     near_plane = 0.1f;  // Near clipping plane
          float     far_plane = 100.0f; // Far clipping plane
          glm::mat4 projection_matrix = glm::perspective(glm::radians(fov),
                                                         aspect_ratio,
                                                         near_plane,
                                                         far_plane);

          combined_matrix = projection_matrix * view_matrix *
                            transalation_matrix * rotation_matrix *
                            scale_matrix;
        }

        GLuint model_matrix_location = glGetUniformLocation(shader_id,
                                                            "modelMatrix");
        glUniformMatrix4fv(model_matrix_location,
                           1,
                           GL_FALSE,
                           glm::value_ptr(combined_matrix));

        glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
        glPopMatrix();

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        hesiod::viewer::unbind_framebuffer();
      }
    }
  }
}

void ViewTree::update_view3d_basemesh()
{
  LOG_DEBUG("here");
  glBindVertexArray(this->vertex_array_id);

  hesiod::viewer::generate_basemesh(this->shape_view3d,
                                    this->vertices,
                                    this->colors);

  hesiod::viewer::create_framebuffer(this->FBO,
                                     this->RBO,
                                     this->image_texture_view3d,
                                     (float)this->shape_view3d.x,
                                     (float)this->shape_view3d.y);
}

} // namespace hesiod::vnode
