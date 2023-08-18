/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
#include "imnodes.h"
#include "macrologger.h"

#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

Link::Link(){};

Link::Link(std::string node_id_from,
           std::string port_id_from,
           int         port_hash_id_from,
           std::string node_id_to,
           std::string port_id_to,
           int         port_hash_id_to)
    : node_id_from(node_id_from), port_id_from(port_id_from),
      port_hash_id_from(port_hash_id_from), node_id_to(node_id_to),
      port_id_to(port_id_to), port_hash_id_to(port_hash_id_to)
{
  LOG_DEBUG("new link [%s]/[%s] to [%s]/[%s]",
            node_id_from.c_str(),
            port_id_from.c_str(),
            node_id_to.c_str(),
            port_id_to.c_str());
}

ViewTree::ViewTree(std::string     id,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap)
    : gnode::Tree(id), shape(shape), tiling(tiling), overlap(overlap)
{
}

Link *ViewTree::get_link_ref_by_id(int link_id)
{
  if (this->links.contains(link_id))
    return &(this->links[link_id]);
  else
  {
    LOG_ERROR("link id [%d] is not known", link_id);
    throw std::runtime_error("unknonw link Id");
  }
}

std::string ViewTree::get_new_id()
{
  return std::to_string(this->id_counter++);
}

void ViewTree::generate_all_links(bool force_update)
{
  LOG_DEBUG("generating Links...");

  for (auto &[id, cnode] : this->get_nodes_map())
    // scan control node outputs
    for (auto &[port_id, port] : cnode.get()->get_ports())
      if ((port.direction == gnode::direction::out) & port.is_connected)
      {
        // inputs are uniquely linked so that we can use their hash_id
        // for the link
        int link_id = port.p_linked_port->hash_id;

        if (force_update or (!this->links.contains(link_id)))
        {
          Link link = Link(id,
                           port_id,
                           port.hash_id,
                           port.p_linked_node->id,
                           port.p_linked_port->id,
                           port.p_linked_port->hash_id);
          this->links[link_id] = link;
        }
      }
}

void ViewTree::generate_all_view_nodes(bool force_update)
{
  LOG_DEBUG("generating ViewNodes...");

  for (auto &[id, cnode] : this->get_nodes_map())
    if (force_update or (!this->get_nodes_map().contains(id)))
      this->generate_view_node_from_control_node(id);
}

void ViewTree::new_link(int port_hash_id_from, int port_hash_id_to)
{
  LOG_DEBUG("%d -> %d", port_hash_id_from, port_hash_id_to);

  // find corresponding nodes
  std::string node_id_from;
  std::string port_id_from;
  std::string node_id_to;
  std::string port_id_to;

  this->get_ids_by_port_hash_id(port_hash_id_from, node_id_from, port_id_from);
  this->get_ids_by_port_hash_id(port_hash_id_to, node_id_to, port_id_to);

  // check if links already exist for the ports that are to be
  // connected
  if (this->get_node_ref_by_id(node_id_from)
          ->get_port_ref_by_id(port_id_from)
          ->is_connected)
  {
    LOG_DEBUG("already connected, disconnecting [%s] / [%s]",
              node_id_from.c_str(),
              port_id_from.c_str());

    // the link Id for the view tree is actually the hash id of the
    // port on the end of the link (since the 'from' port is an output
    // port), we use it to remove the link from the link map
    int hid = this->get_node_ref_by_id(node_id_from)
                  ->get_port_ref_by_id(port_id_from)
                  ->p_linked_port->hash_id;

    this->remove_link(hid);
  }

  if (this->get_node_ref_by_id(node_id_to)
          ->get_port_ref_by_id(port_id_to)
          ->is_connected)
  {
    LOG_DEBUG("already connected, disconnecting [%s] / [%s]",
              node_id_to.c_str(),
              port_id_to.c_str());

    // the link Id for the view tree is actually the hash id of the
    // port on this side of the link (since this is an input port), we
    // use it to remove the link from the link map
    int hid = this->get_node_ref_by_id(node_id_to)
                  ->get_port_ref_by_id(port_id_to)
                  ->hash_id;

    this->remove_link(hid);
  }

  // generate link (in GNode)
  this->link(node_id_from, port_id_from, node_id_to, port_id_to);

  // use input hash id for the link id
  Link link = Link(node_id_from,
                   port_id_from,
                   port_hash_id_from,
                   node_id_to,
                   port_id_to,
                   port_hash_id_to);

  int link_id = port_hash_id_to;
  this->links[link_id] = link;

  // propagate from the source
  this->update_node(node_id_from);
}

void ViewTree::remove_link(int link_id)
{
  Link *p_link = this->get_link_ref_by_id(link_id);
  this->unlink(p_link->node_id_from,
               p_link->port_id_from,
               p_link->node_id_to,
               p_link->port_id_to);

  // only nodes downstream "from" node (excluded) are affected
  this->update_node(p_link->node_id_to);

  // eventually remove link from the link directory
  this->links.erase(link_id);
}

void ViewTree::remove_view_node(std::string node_id)
{

  // for the TreeView, we need to do everything by hand: first remove
  // all the links from and to the node
  std::vector<int> link_id_to_remove = {};

  for (auto &[link_id, link] : this->links)
    if ((link.node_id_from == node_id) | (link.node_id_to == node_id))
      link_id_to_remove.push_back(link_id);

  for (auto &link_id : link_id_to_remove)
    this->remove_link(link_id);

  // remove view node
  if (this->get_nodes_map().contains(node_id))
  {
    LOG_DEBUG("erase view node");
    this->get_nodes_map().erase(node_id);
  }
  else
  {
    LOG_ERROR("view node id [%s] is not known", node_id.c_str());
    throw std::runtime_error("unknonw node Id");
  }

  for (auto &[id, n] : this->get_nodes_map())
    LOG_DEBUG("id: %s", id.c_str());

  // for the control node handled by GNode, everything is taken care
  // of by this method
  this->remove_node(node_id);
}

void ViewTree::render_links()
{
  for (auto &[link_id, link] : this->links)
    ImNodes::Link(link_id, link.port_hash_id_from, link.port_hash_id_to);
}

void ViewTree::render_new_node_treeview()
{
  const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
  const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

  static ImGuiTableFlags flags = ImGuiTableFlags_Reorderable |
                                 ImGuiTableFlags_Hideable |
                                 // ImGuiTableFlags_Sortable |
                                 // ImGuiTableFlags_SortMulti |
                                 // ImGuiTableFlags_RowBg |
                                 ImGuiTableFlags_BordersOuter |
                                 ImGuiTableFlags_BordersV |
                                 ImGuiTableFlags_NoBordersInBody |
                                 ImGuiTableFlags_ScrollY;

  if (ImGui::BeginTable("table_sorting",
                        2,
                        flags,
                        ImVec2(0.f, 0.f), // TEXT_BASE_HEIGHT * 15),
                        0.f))
  {
    ImGui::TableSetupColumn("Type",
                            ImGuiTableColumnFlags_DefaultSort |
                                ImGuiTableColumnFlags_WidthFixed,
                            0.f,
                            0);
    ImGui::TableSetupColumn("Category",
                            ImGuiTableColumnFlags_WidthFixed,
                            0.f,
                            1);
    ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
    ImGui::TableHeadersRow();

    ImGuiListClipper clipper;
    clipper.Begin((int)hesiod::cnode::category_mapping.size());

    while (clipper.Step())
    {
      int k = 0;
      for (auto &[node_type, node_category] : hesiod::cnode::category_mapping)
      {
        if ((k >= clipper.DisplayStart) & (k < clipper.DisplayEnd))
        {
          ImGui::PushID(k);
          ImGui::TableNextRow();
          ImGui::TableNextColumn();

          const bool item_is_selected = false;

          if (ImGui::Selectable(node_type.c_str(),
                                item_is_selected,
                                ImGuiSelectableFlags_SpanAllColumns))
          {
            LOG_DEBUG("selected node type: %s", node_type.c_str());
            this->add_view_node(node_type);
            this->print_node_list();
          }

          ImGui::TableNextColumn();
          ImGui::TextUnformatted(node_category.c_str());
          ImGui::PopID();
        }
        k++;
      }
    }
    ImGui::EndTable();
  }
}

void ViewTree::render_node_list()
{
  for (auto &[id, vnode] : this->get_nodes_map())
    ImGui::Text("id: %s, hash_id: %d", id.c_str(), vnode.get()->hash_id);
}

void ViewTree::render_view_nodes()
{
  for (auto &[id, node] : this->get_nodes_map())
  {
    hesiod::vnode::ViewControlNode *p_vnode =
        (hesiod::vnode::ViewControlNode *)(node.get());
    p_vnode->render_node();
  }
}

void ViewTree::render_settings(std::string node_id)
{
  hesiod::vnode::ViewControlNode *p_vnode =
      (hesiod::vnode::ViewControlNode *)this->get_node_ref_by_id(node_id);
  p_vnode->render_settings();
}

} // namespace hesiod::vnode
