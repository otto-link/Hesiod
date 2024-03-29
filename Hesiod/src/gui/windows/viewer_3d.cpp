/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui_internal.h"
#include "macrologger.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "hesiod/control_node.hpp"
#include "hesiod/render.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/widgets.hpp"
#include "hesiod/windows.hpp"

namespace hesiod::gui
{

Viewer3D::Viewer3D(hesiod::vnode::ViewTree *p_vtree) : p_vtree(p_vtree)
{
  // shader setup
  this->shader_id = hesiod::render::load_shaders(
      "SimpleVertexShader.vertexshader",
      "SimpleFragmentShader.fragmentshader");

  glGenVertexArrays(1, &this->vertex_array_id);
  glGenBuffers(1, &this->vertex_buffer);
  glGenBuffers(1, &this->color_buffer);

  this->update_basemesh();
}

Viewer3D::~Viewer3D()
{
  LOG_DEBUG("Viewer3D::~Viewer3D");

  // clean-up the post-update callbacks
  this->p_vtree->post_update_callbacks.erase(this->get_unique_id());

  // clean-up OpenGL buffers
  glDeleteBuffers(1, &this->vertex_buffer);
  glDeleteBuffers(1, &this->color_buffer);
  glDeleteVertexArrays(1, &this->vertex_array_id);
  glDeleteFramebuffers(1, &this->FBO);
  glDeleteFramebuffers(1, &this->RBO);
}

ShortcutGroupId Viewer3D::get_element_shortcut_group_id()
{
  return "GroupViewer3D" + this->get_unique_id();
}

bool Viewer3D::initialize()
{
  LOG_DEBUG("initializing Viewer3D window [%s] for ViewTree [%s]",
            this->get_unique_id().c_str(),
            p_vtree->id.c_str());

  this->title = "Viewer (3D) " + p_vtree->id;
  this->flags = ImGuiWindowFlags_MenuBar;

  // setup the post-update callback
  this->p_vtree->post_update_callbacks[this->get_unique_id()] = std::bind(
      &Viewer3D::post_update,
      this);

  // force update to generate before-hand any rendering if node data are
  // available
  this->post_update();

  return true;
}

void Viewer3D::post_update()
{
  // retrieve current selection (if any or unless the current node is
  // pinned for this viewer)
  if (!this->pin_current_node)
  {
    this->viewer_node_id = "";

    if (this->p_vtree->get_selected_node_hid().size() > 0)
    {
      uint hash_id = this->p_vtree->get_selected_node_hid().back().Get();
      this->viewer_node_id = this->p_vtree->get_node_id_by_hash_id(hash_id);
    }
  }
  else
  {
    // if the view is pinned but the node has been destroyed
    // in-between, un-pin the view...
    if (!this->p_vtree->is_node_id_in_keys(this->viewer_node_id))
      this->pin_current_node = false;
  }

  this->update_image_texture();
}

bool Viewer3D::render_content()
{
  ImGui::PushID((void *)this);

  ImGui::Text("%s", this->viewer_node_id.c_str());

  if (this->p_vtree->is_node_id_in_keys(this->viewer_node_id))
  {

    hesiod::vnode::ViewNode *p_vnode =
        this->p_vtree->get_node_ref_by_id<hesiod::vnode::ViewNode>(
            this->viewer_node_id);

    if (p_vnode->get_view3d_elevation_port_id() != "")
    {
      ImGui::SameLine();
      ImGui::Checkbox("Pin this node", &this->pin_current_node);

      if (ImGui::Button("Top"))
      {
        alpha_x = 90.f;
        alpha_y = 0.f;
        delta_x = 0.f;
        delta_y = 0.f;
        scale = 1.f;
        this->update_image_texture(false);
      }
      ImGui::SameLine();

      if (ImGui::Checkbox("Wireframe", &this->wireframe))
        this->update_image_texture(false);
      ImGui::SameLine();

      ImGui::Checkbox("Auto rotate", &this->auto_rotate);
      if (this->auto_rotate)
        this->update_image_texture(false);

      if (ImGui::SliderFloat("h_scale", &this->h_scale, 0.f, 2.f, "%.2f"))
        this->update_image_texture(false);

      if (hesiod::gui::select_shape("Display shape",
                                    this->display_shape,
                                    this->p_vtree->get_shape()))
      {
        this->update_basemesh();
        this->update_image_texture();
      }

      // --- 3D rendering viewport
      ImGuiIO &io = ImGui::GetIO();

      float window_width = ImGui::GetContentRegionAvail().x;
      {
        ImVec2 pos = ImGui::GetCursorScreenPos();

        ImVec2 p0 = ImVec2(pos.x, pos.y);
        ImVec2 p1 = ImVec2(pos.x + window_width, pos.y + window_width);

        ImDrawList *draw_list = ImGui::GetWindowDrawList();

        draw_list->AddImage((void *)(intptr_t)this->image_texture,
                            p0,
                            p1,
                            ImVec2(0, 1),
                            ImVec2(1, 0));
        draw_list->AddRect(p0, p1, IM_COL32(255, 255, 255, 255));
        ImGui::InvisibleButton("##image3d", ImVec2(-1, -1));

        // if (this->show_view3d_on_background)
        // {
        //   float display_width = std::min(io.DisplaySize.x, io.DisplaySize.y);

        //   ImGui::GetBackgroundDrawList()->AddImage(
        //       (void *)(intptr_t)this->image_texture,
        //       ImVec2(0, 0),
        //       ImVec2(display_width, display_width),
        //       ImVec2(0, 1),
        //       ImVec2(1, 0));
        // }
      }

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
            this->update_image_texture(false);
          }

          if (ImGui::IsMouseDown(0))
          {
            this->alpha_y += 100.f * factor * io.MouseDelta.x / window_width;
            this->alpha_x += 100.f * factor * io.MouseDelta.y / window_width;
            this->update_image_texture(false);
          }
          if (ImGui::IsMouseDown(2))
          {
            this->delta_x += 4.f * factor * io.MouseDelta.x / window_width;
            this->delta_y -= 4.f * factor * io.MouseDelta.y / window_width;
            this->update_image_texture(false);
          }
        }
      }
    }
  }
  ImGui::PopID();

  return true;
}

void Viewer3D::update_basemesh()
{
  glBindVertexArray(this->vertex_array_id);

  hesiod::render::generate_basemesh(this->display_shape,
                                    this->vertices,
                                    this->colors);

  hesiod::render::create_framebuffer(this->FBO,
                                     this->RBO,
                                     this->image_texture,
                                     (float)this->display_shape.x,
                                     (float)this->display_shape.y);
}

void Viewer3D::update_image_texture(bool vertex_array_update)
{
  if (this->p_vtree->is_node_id_in_keys(this->viewer_node_id))
  {
    hesiod::vnode::ViewNode *p_vnode =
        this->p_vtree->get_node_ref_by_id<hesiod::vnode::ViewNode>(
            this->viewer_node_id);

    std::string elevation_pid = p_vnode->get_view3d_elevation_port_id();
    std::string color_pid = p_vnode->get_view3d_color_port_id();

    if (elevation_pid != "")
    {

      void *p_elev = p_vnode->get_p_data(elevation_pid);
      void *p_color = color_pid == "" ? nullptr
                                      : p_vnode->get_p_data(color_pid);

      if (p_elev) // elevation data are actually available
      {
        if (vertex_array_update)
        {
          // TODO extend to arrays
          hmap::HeightMap *p_h = (hmap::HeightMap *)p_elev;
          hmap::Array      z = p_h->to_array(this->display_shape);

          hesiod::render::update_vertex_elevations(z, this->vertices);

          if (!p_color)
            // color is undefined (on purpose or data not available),
            // only solid white and hillshading
            hesiod::render::update_vertex_colors(z, this->colors);
          else
          {
            switch (p_vnode->get_port_ref_by_id(color_pid)->dtype)
            {
            case hesiod::cnode::dtype::dHeightMap:
            {
              // the color data are provided as an heightmap => it
              // is assigned to the Red channel, other channels are
              // set to zero
              hmap::HeightMap *p_c = (hmap::HeightMap *)p_color;
              hmap::Array      c = 1.f - p_c->to_array(this->display_shape);
              hesiod::render::update_vertex_colors(z, c, this->colors);
            }
            break;

            case hesiod::cnode::dtype::dHeightMapRGBA:
            {
              hmap::HeightMapRGBA *p_c = (hmap::HeightMapRGBA *)p_color;
              hmap::Array r = p_c->rgba[0].to_array(this->display_shape);
              hmap::Array g = p_c->rgba[1].to_array(this->display_shape);
              hmap::Array b = p_c->rgba[2].to_array(this->display_shape);
              hmap::Array a = p_c->rgba[3].to_array(this->display_shape);

              hesiod::render::update_vertex_colors(z,
                                                   r,
                                                   g,
                                                   b,
                                                   this->colors,
                                                   &a);
            }
            break;

            case hesiod::cnode::dtype::dPath:
            {
              // project path to an array (with 0 and 1 only)
              hmap::Path *p_path = (hmap::Path *)p_color;
              if (p_path->get_npoints() > 1)
              {
                hmap::Array c = hmap::Array(this->display_shape);
                hmap::Path  path_copy = *p_path;
                path_copy.set_values(1.f);
                hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
                path_copy.to_array(c, bbox);
                c = 1.f - c;
                hesiod::render::update_vertex_colors(z, c, this->colors);
              }
              else
                hesiod::render::update_vertex_colors(z, this->colors);
            }
            break;

            default:
              // render a black surface
              for (auto &v : this->colors)
                v = 0.f;
            }
          }

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

        hesiod::render::bind_framebuffer(this->FBO);

        glUseProgram(this->shader_id);
        glClearColor(clear_color.Value.x,
                     clear_color.Value.y,
                     clear_color.Value.z,
                     clear_color.Value.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // reset glViewport and scale framebuffer
        glViewport(0, 0, this->display_shape.x, this->display_shape.y);

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
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);

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

          // define perspective projection parameters
          glm::mat4 projection_matrix = glm::perspective(
              glm::radians(this->fov),
              this->aspect_ratio,
              this->near_plane,
              this->far_plane);

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

        hesiod::render::unbind_framebuffer();
      }
    }
  }
}

} // namespace hesiod::gui
