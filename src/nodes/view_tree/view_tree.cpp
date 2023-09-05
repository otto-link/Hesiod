/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
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
  ax::NodeEditor::Config config;
  config.SettingsFile = (id + ".json").c_str();
  config.NavigateButtonIndex = 2;
  config.ContextMenuButtonIndex = 1;
  this->p_node_editor_context = ax::NodeEditor::CreateEditor(&config);
}

ViewTree::~ViewTree()
{
  // shutdown node editor
  ax::NodeEditor::DestroyEditor(this->p_node_editor_context);
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

std::string ViewTree::get_node_type(std::string node_id)
{
  return this->get_node_ref_by_id(node_id)->get_node_type();
}

void ViewTree::insert_clone_node(std::string node_id)
{
  std::string new_node_id = this->add_view_node("Clone");
  this->render_view_node(new_node_id);

  ImVec2 pos = ax::NodeEditor::GetNodePosition(
      this->get_node_ref_by_id(node_id)->hash_id);
  ax::NodeEditor::SetNodePosition(
      this->get_node_ref_by_id(new_node_id)->hash_id,
      ImVec2(pos.x + 200.f, pos.y));

  // automatic reconnection (only if there is an output named
  // "output")
  gnode::Node *p_node = this->get_node_ref_by_id(node_id);
  gnode::Node *p_clone = this->get_node_ref_by_id(new_node_id);

  if (p_node->is_port_id_in_keys("output"))
  {
    // hash id of the port on the other end of the link (if
    // connected)
    int port_hash_id_to = 0;

    bool previously_connected = false;
    if (p_node->get_port_ref_by_id("output")->is_connected)
    {
      port_hash_id_to =
          p_node->get_port_ref_by_id("output")->p_linked_port->hash_id;
      previously_connected = true;
    }

    // selected node -> new Clone node
    this->new_link(p_node->get_port_ref_by_id("output")->hash_id,
                   p_clone->get_port_ref_by_id("input")->hash_id);

    // new Clone node -> previously connected input (if any)
    if (previously_connected)
      this->new_link(p_clone->get_port_ref_by_id("thru##0")->hash_id,
                     port_hash_id_to);
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

  // do anything only if the ports do not have the same direction (to
  // avoid linking two inputs or two outputs) and have the same data
  // type
  if (this->get_node_ref_by_id(node_id_from)
              ->get_port_ref_by_id(port_id_from)
              ->direction != this->get_node_ref_by_id(node_id_to)
                                 ->get_port_ref_by_id(port_id_to)
                                 ->direction &&
      this->get_node_ref_by_id(node_id_from)
              ->get_port_ref_by_id(port_id_from)
              ->dtype == this->get_node_ref_by_id(node_id_to)
                             ->get_port_ref_by_id(port_id_to)
                             ->dtype)
  {

    // swap from and to if the link has been drawn from an input to and
    // output (in GNode the links are created always from the output to
    // the input)
    if (this->get_node_ref_by_id(node_id_from)
            ->get_port_ref_by_id(port_id_from)
            ->direction == gnode::direction::in)
    {
      node_id_from.swap(node_id_to);
      port_id_from.swap(port_id_to);
    }

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
