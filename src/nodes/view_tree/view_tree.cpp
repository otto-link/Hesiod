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

ViewTree::ViewTree(gnode::Tree    *p_control_tree,
                   hmap::Vec2<int> shape,
                   hmap::Vec2<int> tiling,
                   float           overlap)
    : p_control_tree(p_control_tree), shape(shape), tiling(tiling),
      overlap(overlap)
{
  this->id = this->p_control_tree->id;
  this->label = this->p_control_tree->label;
}

std::string ViewTree::get_control_node_id_by_hash_id(int control_node_hash_id)
{
  gnode::Node *p_node = this->get_control_node_ref_by_hash_id(
      control_node_hash_id);
  return p_node->id;
}

gnode::Node *ViewTree::get_control_node_ref_by_hash_id(int control_node_hash_id)
{
  gnode::Node *p_cnode = nullptr;

  // scan control nodes and their ports to find the
  for (auto &[id, cnode] : this->get_control_nodes_map())
    if (cnode.get()->hash_id == control_node_hash_id)
    {
      p_cnode = cnode.get();
      break;
    }

  if (!p_cnode)
  {
    LOG_ERROR("node hash id [%d] is not known", control_node_hash_id);
    throw std::runtime_error("unknonw node hash_id");
  }

  return p_cnode;
}

GNodeMapping ViewTree::get_control_nodes_map()
{
  return this->p_control_tree->get_nodes_map();
}

void ViewTree::get_ids_by_port_hash_id(int          port_hash_id,
                                       std::string &node_id,
                                       std::string &port_id)
{
  // scan control nodes and their ports to find the
  for (auto &[nid, cnode] : this->get_control_nodes_map())
    for (auto &[pid, port] : cnode.get()->get_ports())
      if (port.hash_id == port_hash_id)
      {
        node_id = nid;
        port_id = pid;
        break;
      }
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

ViewNodeMapping ViewTree::get_view_nodes_map()
{
  return this->view_nodes_mapping;
}

ViewNode *ViewTree::get_view_node_ref_by_id(std::string node_id)
{
  if (this->view_nodes_mapping.contains(node_id))
  {
    return this->view_nodes_mapping[node_id].get();
  }
  else
  {
    LOG_ERROR("view node id [%s] is not known", node_id.c_str());
    throw std::runtime_error("unknonw node Id");
  }
}

void ViewTree::generate_all_links(bool force_update)
{
  LOG_DEBUG("generating Links...");

  for (auto &[id, cnode] : this->get_control_nodes_map())
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

  for (auto &[id, cnode] : this->get_control_nodes_map())
    if (force_update or (!this->view_nodes_mapping.contains(id)))
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

  // generate link (in GNode)
  this->p_control_tree->link(node_id_from,
                             port_id_from,
                             node_id_to,
                             port_id_to);

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
  this->p_control_tree->unlink(p_link->node_id_from,
                               p_link->port_id_from,
                               p_link->node_id_to,
                               p_link->port_id_to);

  // only nodes downstream "from" node (excluded) are affected
  this->update_node(p_link->node_id_to);

  // eventually remove link from the link directory
  this->links.erase(link_id);
}

void ViewTree::remove_node(std::string node_id)
{

  // for the TreeView, we need to do everything by hand: first remove
  // all the links from and to the node
  for (auto &[link_id, link] : this->links)
  {
    if ((link.node_id_from == node_id) | (link.node_id_to == node_id))
      this->remove_link(link_id);
  }

  // remove view node
  if (this->get_view_nodes_map().contains(node_id))
  {
    LOG_DEBUG("erase view node");
    this->view_nodes_mapping.erase(node_id);
  }
  else
  {
    LOG_ERROR("view node id [%s] is not known", node_id.c_str());
    throw std::runtime_error("unknonw node Id");
  }

  for (auto &[id, n] : this->view_nodes_mapping)
    LOG_DEBUG("id: %s", id.c_str());

  // for the control node handled by GNode, everything is taken care
  // of by this method
  this->p_control_tree->remove_node(node_id);
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
            this->add_node(node_type);
            this->p_control_tree->print_node_list();
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
  for (auto &[id, vnode] : this->view_nodes_mapping)
  {
    ImGui::Text("id: %s, hash_id: %d",
                id.c_str(),
                vnode.get()->get_p_control_node()->hash_id);
  }
}

void ViewTree::render_view_nodes()
{
  for (auto &[id, vnode] : this->view_nodes_mapping)
    vnode.get()->render_node();
}

void ViewTree::render_settings(std::string node_id)
{
  this->get_view_node_ref_by_id(node_id)->render_settings();
}

void ViewTree::update()
{
  this->p_control_tree->update();
}

void ViewTree::update_node(std::string node_id)
{
  this->p_control_tree->update_node(node_id);
}

} // namespace hesiod::vnode
