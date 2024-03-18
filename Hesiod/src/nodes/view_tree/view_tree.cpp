/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
#include "hesiod/shortcuts.hpp"
#include "macrologger.h"

#include "hesiod/render.hpp"

#include "hesiod/control_tree.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"
#include "hesiod/windows.hpp"

namespace hesiod::vnode
{

LinkInfos *ViewTree::get_link_ref_by_id(int link_id)
{
  if (this->links_infos.contains(link_id))
    return &(this->links_infos[link_id]);
  else
  {
    LOG_ERROR("link id [%d] is not known", link_id);
    throw std::runtime_error("unknonw link Id");
  }
}

ImU32 ViewTree::get_node_color(std::string node_id) const
{
  std::string node_type = this->get_node_type(node_id);
  std::string node_category = hesiod::cnode::category_mapping.at(node_type);
  std::string main_category = node_category.substr(0, node_category.find("/"));
  return ImColor(category_colors.at(main_category).hovered);
}

void ViewTree::set_sto(hmap::Vec2<int> new_shape,
                       hmap::Vec2<int> new_tiling,
                       float           new_overlap)
{
  // TODO quick and dirty, did this only to allow modifications in the
  // main GUI for demo purpose
  this->shape = new_shape;
  this->tiling = new_tiling;
  this->overlap = new_overlap;
}

std::string ViewTree::add_view_node(std::string control_node_type,
                                    std::string node_id)
{
  std::string id;
  if (node_id == "")
    id = control_node_type + "##" + this->get_new_id();
  else
    id = node_id;

  LOG_DEBUG("adding node type: %s", control_node_type.c_str());

  this->add_node(create_view_node_from_type(control_node_type,
                                            id,
                                            this->shape,
                                            this->tiling,
                                            this->overlap));
  return id;
}

void ViewTree::clear()
{
  this->json_filename = "";
  this->remove_all_nodes();
  this->links_infos.clear();
}

void ViewTree::set_selected_node_hid(
    std::vector<ax::NodeEditor::NodeId> new_selected_node_hid)
{
  // if the selection has changed, trigger the post-update callbacks
  // to propagate the info to other windows
  bool do_post_update = false;

  if (this->selected_node_hid.size() != new_selected_node_hid.size())
    do_post_update = true;

  if (this->selected_node_hid.size() > 0 && new_selected_node_hid.size() > 0)
    if (this->selected_node_hid.back() != new_selected_node_hid.back())
      do_post_update = true;

  // backup node selection
  this->selected_node_hid = new_selected_node_hid;

  if (do_post_update)
    this->post_update();
}

void ViewTree::new_link(std::string node_id_from,
                        std::string port_id_from,
                        std::string node_id_to,
                        std::string port_id_to)
{
  // --- check if linking can indeed be made
  bool do_link = true;

  // prevent a node from linking with itself
  do_link &= node_id_from != node_id_to;

  // do anything only if the ports do not have the same direction (to
  // avoid linking two inputs or two outputs) and have the same data
  // type
  do_link &= this->get_node_ref_by_id(node_id_from)
                 ->get_port_ref_by_id(port_id_from)
                 ->direction != this->get_node_ref_by_id(node_id_to)
                                    ->get_port_ref_by_id(port_id_to)
                                    ->direction;

  do_link &= this->get_node_ref_by_id(node_id_from)
                 ->get_port_ref_by_id(port_id_from)
                 ->dtype == this->get_node_ref_by_id(node_id_to)
                                ->get_port_ref_by_id(port_id_to)
                                ->dtype;

  if (do_link)
  {

    // swap from and to if the link has been drawn from an input to and
    // output (in GNode the links_infos are created always from the output to
    // the input)
    if (this->get_node_ref_by_id(node_id_from)
            ->get_port_ref_by_id(port_id_from)
            ->direction == gnode::direction::in)
    {
      node_id_from.swap(node_id_to);
      port_id_from.swap(port_id_to);
    }

    int port_hash_id_from = this->get_node_ref_by_id(node_id_from)
                                ->get_port_ref_by_id(port_id_from)
                                ->hash_id;

    int port_hash_id_to = this->get_node_ref_by_id(node_id_to)
                              ->get_port_ref_by_id(port_id_to)
                              ->hash_id;

    // --- check if links_infos already exist for the ports that are to be
    // --- connected
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

    // --- generate link

    // in GNode (data link)
    this->link(node_id_from, port_id_from, node_id_to, port_id_to);

    // use input hash id for the link id
    LinkInfos link = LinkInfos(node_id_from,
                               port_id_from,
                               port_hash_id_from,
                               node_id_to,
                               port_id_to,
                               port_hash_id_to);

    int link_id = port_hash_id_to;
    this->links_infos[link_id] = link;

    if (this->is_cyclic())
    {
      // TODO the resulting tree is cyclic, revert what's just been
      // done above
      LOG_DEBUG("%d", this->is_cyclic());
      throw std::runtime_error("cyclic graph");
    }
    else
      // not cyclic, carry on and propagate from the source
      this->update_node(node_id_to);
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

  this->new_link(node_id_from, port_id_from, node_id_to, port_id_to);
}

void ViewTree::post_update()
{
  for (auto &[key, callback] : this->post_update_callbacks)
    callback();
}

void ViewTree::remove_link(int link_id)
{
  // in GNode (data link)
  LinkInfos *p_link = this->get_link_ref_by_id(link_id);
  this->unlink(p_link->node_id_from,
               p_link->port_id_from,
               p_link->node_id_to,
               p_link->port_id_to);

  // only nodes downstream "from" node (excluded) are affected
  this->update_node(p_link->node_id_to);

  // eventually remove link from the link directory
  this->links_infos.erase(link_id);
}

void ViewTree::remove_view_node(std::string node_id)
{

  // for the TreeView, we need to do everything by hand: first remove
  // all the links_infos from and to the node
  std::vector<int> link_id_to_remove = {};

  for (auto &[link_id, link] : this->links_infos)
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

  // for the control node handled by GNode, everything is taken care
  // of by this method
  this->remove_node(node_id);
}

void ViewTree::render_settings(std::string node_id)
{
  this->get_node_ref_by_id<ViewNode>(node_id)->render_settings();
}

void ViewTree::store_node_positions(
    ax::NodeEditor::EditorContext *p_node_editor_context)
{
  ax::NodeEditor::SetCurrentEditor(p_node_editor_context);

  this->node_positions.clear();
  for (auto &[id, vnode] : this->get_nodes_map())
  {
    ImVec2 pos = ax::NodeEditor::GetNodePosition(vnode.get()->hash_id);
    this->node_positions[id] = pos;
  }

  ax::NodeEditor::SetCurrentEditor(nullptr);
}

} // namespace hesiod::vnode
