/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui_internal.h"
#include "macrologger.h"

#include "hesiod/control_node.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/widgets.hpp"
#include "hesiod/windows.hpp"

namespace hesiod::gui
{

Viewer2D::Viewer2D(hesiod::vnode::ViewTree *p_vtree) : p_vtree(p_vtree)
{
}

Viewer2D::~Viewer2D()
{
  // clean-up the post-update callbacks
  this->p_vtree->post_update_callbacks.erase(this->title +
                                             this->get_unique_id());
}

ShortcutGroupId Viewer2D::get_element_shortcut_group_id()
{
  return "GroupViewer2D" + this->get_unique_id();
}

bool Viewer2D::initialize()
{
  LOG_DEBUG("initializing Viewer2D window [%s] for ViewTree [%s]",
            this->get_unique_id().c_str(),
            p_vtree->id.c_str());

  this->title = "Viewer (2D) " + p_vtree->id;
  this->flags = ImGuiWindowFlags_MenuBar;

  // setup the post-update callback
  this->p_vtree->post_update_callbacks[this->title + this->get_unique_id()] =
      std::bind(&Viewer2D::post_update, this);

  // force update to generate before-hand any rendering if node data are
  // available
  this->post_update();

  return true;
}

void Viewer2D::post_update()
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

bool Viewer2D::render_content()
{
  ImGui::PushID((void *)this);

  ImGui::Text("%s", this->viewer_node_id.c_str());

  if (this->p_vtree->is_node_id_in_keys(this->viewer_node_id))
  {

    hesiod::vnode::ViewNode *p_vnode =
        this->p_vtree->get_node_ref_by_id<hesiod::vnode::ViewNode>(
            this->viewer_node_id);

    if (p_vnode->get_preview_port_id() != "")
    {
      ImGui::SameLine();
      ImGui::Checkbox("Pin this node", &this->pin_current_node);

      if (ImGui::Button("Reset view"))
      {
        this->zoom = 100.f;
        this->uv0 = {0.f, 0.f};
      }
      ImGui::SameLine();

      if (hesiod::gui::listbox_map_enum(this->cmap_map, this->cmap, 128.f))
        this->update_image_texture();

      ImGui::SameLine();
      if (ImGui::Checkbox("Hillshading", &this->hillshade))
        this->update_image_texture();

      if (hesiod::gui::select_shape("Display shape",
                                    this->display_shape,
                                    this->p_vtree->get_shape()))
      {
        this->update_image_texture();
      }

      float  window_width = ImGui::GetContentRegionAvail().x;
      ImVec2 pos = ImGui::GetCursorScreenPos();

      ImVec2 uv1 = {this->uv0[0] + 100.f / this->zoom,
                    this->uv0[1] + 100.f / this->zoom};

      ImVec2 p0 = ImVec2(pos.x, pos.y);
      ImVec2 p1 = ImVec2(pos.x + window_width, pos.y + window_width);

      ImDrawList *draw_list = ImGui::GetWindowDrawList();

      draw_list->AddRectFilled(p0, p1, IM_COL32(50, 50, 50, 255));
      draw_list->AddImage((void *)(intptr_t)this->image_texture,
                          p0,
                          p1,
                          this->uv0,
                          uv1);
      draw_list->AddRect(p0, p1, IM_COL32(255, 255, 255, 255));

      ImGui::InvisibleButton("##image2d", ImVec2(-1, -1));

      // zooming and panning
      ImGuiIO &io = ImGui::GetIO();
      {

        ImGui::SetItemKeyOwner(ImGuiKey_MouseLeft);
        ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);
        if (ImGui::IsItemHovered())
        {
          // zoom
          if (io.MouseWheel)
            this->zoom = std::max(1.f,
                                  this->zoom * (1.f + 0.05f * io.MouseWheel));

          // position
          if (ImGui::IsMouseDown(2))
          {
            ImVec2 window_size = ImGui::GetWindowSize();
            float  du = -io.MouseDelta.x / window_size[0];
            float  dv = -io.MouseDelta.y / window_size[1];
            this->uv0 = {this->uv0[0] + du, this->uv0[1] + dv};
          }
        }
      }
    }
  }
  ImGui::PopID();

  return true;
}

void Viewer2D::update_image_texture()
{
  // update preview
  if (this->p_vtree->is_node_id_in_keys(this->viewer_node_id))
  {
    hesiod::vnode::ViewNode *p_vnode =
        this->p_vtree->get_node_ref_by_id<hesiod::vnode::ViewNode>(
            this->viewer_node_id);

    std::string data_pid = p_vnode->get_preview_port_id();

    if (data_pid != "")
    {
      void *p_data = p_vnode->get_p_data(data_pid);

      if (p_data)
      {
        std::vector<uint8_t> img = {};

        switch (p_vnode->get_port_ref_by_id(data_pid)->dtype)
        {
        case hesiod::cnode::dtype::dHeightMap:
        {
          hmap::HeightMap *p_h = (hmap::HeightMap *)p_data;
          hmap::Array      array = p_h->to_array(this->display_shape);
          img = hmap::colorize(array,
                               array.min(),
                               array.max(),
                               this->cmap,
                               this->hillshade);
        }
        break;

        case hesiod::cnode::dtype::dHeightMapRGB:
        {
          hmap::HeightMapRGB *p_c = (hmap::HeightMapRGB *)p_data;
          if (p_c->shape.x > 0)
            img = p_c->to_img_8bit(this->display_shape);
        }
        break;

        case hesiod::cnode::dtype::dArray:
        {
          hmap::Array array =
              ((hmap::Array *)p_data)->resample_to_shape(this->display_shape);
          img = hmap::colorize(array,
                               array.min(),
                               array.max(),
                               this->cmap,
                               this->hillshade);
        }
        break;

        default:
          LOG_ERROR("data type not suitable for 2d viewer");
        }

        hesiod::vnode::img_to_texture_rgb(img,
                                          this->display_shape,
                                          this->image_texture);
      }
    }
  }
}

} // namespace hesiod::gui
