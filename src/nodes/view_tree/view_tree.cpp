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
  // initialize node editor
  this->p_node_editor_context = ImNodes::EditorContextCreate();

  ImNodes::PushAttributeFlag(
      ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

  ImNodesIO &io = ImNodes::GetIO();
  io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
  io.MultipleSelectModifier.Modifier = &ImGui::GetIO().KeyCtrl;
}

ViewTree::~ViewTree()
{
  // shutdown node editor
  ImNodes::EditorContextFree(this->p_node_editor_context);
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

hesiod::vnode::ViewControlNode *ViewTree::get_view_control_node_ref_by_id(
    std::string node_id)
{
  return (hesiod::vnode::ViewControlNode *)(this->get_node_ref_by_id(node_id));
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

void ViewTree::set_view2d_node_id(std::string node_id)
{
  if (node_id != this->view2d_node_id)
  {
    // reset previous node
    if (this->is_node_id_in_keys(this->view2d_node_id))
    {
      this->get_view_control_node_ref_by_id(this->view2d_node_id)
          ->set_show_view2d(false);
    }

    this->view2d_node_id = node_id;
    this->get_view_control_node_ref_by_id(this->view2d_node_id)
        ->set_show_view2d(true);
    this->get_view_control_node_ref_by_id(this->view2d_node_id)
        ->update_preview();
  }
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

  if (this->is_cyclic())
  {
    // TODO the resulting tree is cyclic, revert what's just been
    // done above
    LOG_DEBUG("%d", this->is_cyclic());
    throw std::runtime_error("cyclic graph");
  }
  else
    // not cyclic, carry on and propagate from the source
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

} // namespace hesiod::vnode
