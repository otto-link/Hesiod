/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "hesiod/control_node.hpp"

namespace hesiod::vnode
{

typedef std::map<std::string, std::shared_ptr<gnode::Node>> GNodeMapping;

typedef std::map<std::string, std::shared_ptr<hesiod::vnode::ViewNode>>
    ViewNodeMapping;

struct Link
{
  // related to GNode and Hesiod
  std::string node_id_from;
  std::string port_id_from;
  int         port_hash_id_from;
  std::string node_id_to;
  std::string port_id_to;
  int         port_hash_id_to;

  Link();

  Link(std::string node_id_from,
       std::string port_id_from,
       int         port_hash_id_from,
       std::string node_id_to,
       std::string port_id_to,
       int         port_hash_id_to);
};

class ViewTree
{
public:
  std::string id;
  std::string label;

  ViewTree(gnode::Tree *p_control_tree);

  GNodeMapping get_control_nodes_map();

  Link *get_link_ref_by_id(int link_id);

  void generate_all_links(bool force_update = false);

  void generate_all_view_nodes(bool force_update = false);

  void generate_view_node_from_control_node(std::string control_node_id);

  void remove_link(int link_id);

  void render_links();

  void render_view_nodes();

  void update();

  // private: // TODO
  gnode::Tree        *p_control_tree;
  ViewNodeMapping     view_nodes_mapping;
  std::map<int, Link> links = {};
};

template <class TControl, class TView>
std::shared_ptr<TView> generate_view_from_control(gnode::Node *p_control_node);

} // namespace hesiod::vnode
