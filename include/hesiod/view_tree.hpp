/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "hesiod/control_node.hpp"

namespace hesiod::vnode
{

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

class ViewTree : public gnode::Tree
{
public:
  ViewTree(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap);

  Link *get_link_ref_by_id(int link_id);

  std::string get_new_id();

  void add_view_node(std::string control_node_type);

  void generate_all_links(bool force_update = false);

  void generate_all_view_nodes(bool force_update = false);

  void generate_view_node_from_control_node(std::string control_node_id);

  void new_link(int port_hash_id_from, int port_hash_id_to);

  void remove_link(int link_id);

  void remove_view_node(std::string node_id);

  void render_links();

  void render_new_node_treeview();

  void render_node_list();

  void render_settings(std::string node_id);

  void render_view_nodes();

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;

  std::map<int, Link> links = {};
  int                 id_counter = 0;
};

} // namespace hesiod::vnode
