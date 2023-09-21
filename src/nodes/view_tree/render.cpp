/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
#include "macrologger.h"
#include <imgui_node_editor.h>

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

void ViewTree::render_links()
{
  for (auto &[link_id, link] : this->links)
  {
    // change color of "dead" links when a node upstream is frozen
    bool is_link_frozen =
        this->get_node_ref_by_id(link.node_id_from)->frozen_outputs;

    ImVec4 color = ImVec4(1.f, 1.f, 1.f, 1.f);
    if (is_link_frozen)
      color = ImVec4(0.6f, 0.2f, 0.2f, 1.f);

    ax::NodeEditor::Link(ax::NodeEditor::LinkId(link_id),
                         ax::NodeEditor::PinId(link.port_hash_id_from),
                         ax::NodeEditor::PinId(link.port_hash_id_to),
                         color);
  }
}

void ViewTree::render_view_node(std::string node_id)
{
  this->get_view_control_node_ref_by_id(node_id)->render_node();
}

void ViewTree::render_view_nodes()
{
  for (auto &[id, node] : this->get_nodes_map())
    this->render_view_node(id);
}

bool ViewTree::render_view2d()
{
  bool has_changed = false;
  ImGui::PushID((void *)this);

  {
    ImGui::Text("%s", this->view2d_node_id.c_str());

    // // TODO listbox of output to display
    // ImGui::SameLine();
    // hesiod::vnode::ViewControlNode *p_vnode =
    //     this->get_view_control_node_ref_by_id(this->view2d_node_id);
    // p_vnode->get_p_data(p_vnode->get_view2d_port_id());
  }

  {
    if (ImGui::Button("256 x 256"))
      this->shape_view2d = {256, 256};
    ImGui::SameLine();
    if (ImGui::Button("512 x 512"))
      this->shape_view2d = {512, 512};
    ImGui::SameLine();
    if (ImGui::Button("1024 x 1024"))
      this->shape_view2d = {1024, 1024};
  }

  if (hesiod::gui::listbox_map_enum(this->cmap_map, this->cmap_view2d, 128.f))
    this->update_image_texture_view2d();

  ImGui::SameLine();
  if (ImGui::Checkbox("Hillshading", &this->hillshade_view2d))
    this->update_image_texture_view2d();

  if (this->image_texture_view2d != 0)
  {
    ImGui::Image((void *)(intptr_t)this->image_texture_view2d,
                 ImVec2(this->shape_view2d.x, this->shape_view2d.y));
  }

  ImGui::PopID();
  return has_changed;
}

void ViewTree::render_settings(std::string node_id)
{
  this->get_view_control_node_ref_by_id(node_id)->render_settings();
}

void ViewTree::update_image_texture_view2d()
{
  if (this->view2d_node_id != "")
  {
    hesiod::vnode::ViewControlNode *p_vnode =
        this->get_view_control_node_ref_by_id(this->view2d_node_id);

    if (p_vnode->get_view2d_port_id() != "")
    {
      void *p_data = p_vnode->get_p_data(p_vnode->get_view2d_port_id());

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
    else
    {
      // 1 pixel black image
      std::vector<uint8_t> img = {0, 0, 0};
      img_to_texture_rgb(img, {1, 1}, this->image_texture_view2d);
    }
  }
}

void ViewTree::automatic_node_layout()
{
  std::vector<gnode::Point> positions = this->compute_graph_layout_sugiyama();
  int                       k = 0;

  for (auto &[node_id, node] : this->get_nodes_map())
  {
    ax::NodeEditor::SetNodePosition(
        this->get_node_ref_by_id(node_id)->hash_id,
        ImVec2(512.f + 256.f * positions[k].x, 256.f + 256.f * positions[k].y));
    k++;
  }
}

} // namespace hesiod::vnode
