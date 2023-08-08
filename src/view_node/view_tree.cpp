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

// Link::Link(std::string node_id_from,
//            std::string port_id_from,
//            std::string node_id_to,
//            std::string port_id_to)
//     : node_id_from(node_id_from), port_id_from(port_id_from),
//       node_id_to(node_id_to), port_id_to(port_id_to)
// {
// }

ViewTree::ViewTree(gnode::Tree *p_control_tree) : p_control_tree(p_control_tree)
{
  this->id = this->p_control_tree->id;
  this->label = this->p_control_tree->label;
}

GNodeMapping ViewTree::get_control_nodes_map()
{
  return this->p_control_tree->get_nodes_map();
}

void ViewTree::generate_all_view_nodes(bool force_update)
{
  LOG_DEBUG("generating ViewNodes...");

  for (auto &[id, cnode] : this->get_control_nodes_map())
    if (force_update or (!this->view_nodes_mapping.contains(id)))
      this->generate_view_node_from_control_node(id);
}

void ViewTree::generate_view_node_from_control_node(std::string control_node_id)
{
  gnode::Node *p_cnode = this->p_control_tree->get_node_ref_by_id(
      control_node_id);

  LOG_DEBUG("control node [%s], type: %s",
            p_cnode->id.c_str(),
            p_cnode->get_node_type().c_str());

  if (p_cnode->get_node_type() == "GammaCorrection")
  {
    this->view_nodes_mapping[p_cnode->id] =
        generate_view_from_control<hesiod::cnode::GammaCorrection,
                                   hesiod::vnode::ViewGammaCorrection>(p_cnode);
  }
  else if (p_cnode->get_node_type() == "Perlin")
  {
    this->view_nodes_mapping[p_cnode->id] =
        generate_view_from_control<hesiod::cnode::Perlin,
                                   hesiod::vnode::ViewPerlin>(p_cnode);
  }
}

void ViewTree::update()
{
  this->p_control_tree->update();
}

template <class TControl, class TView>
std::shared_ptr<TView> generate_view_from_control(gnode::Node *p_gnode_node)
{
  TControl *p_control_node = (TControl *)p_gnode_node;
  return std::make_shared<TView>(p_control_node);
}

} // namespace hesiod::vnode
