/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include "imnodes.h"

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

  ~ViewTree();

  Link *get_link_ref_by_id(int link_id);

  std::string get_new_id();

  std::string add_view_node(std::string control_node_type);

  void generate_all_links(bool force_update = false);

  void generate_all_view_nodes(bool force_update = false);

  void new_link(int port_hash_id_from, int port_hash_id_to);

  void remove_link(int link_id);

  void remove_view_node(std::string node_id);

  void render_links();

  void render_new_node_popup(bool &open_popup);

  std::string render_new_node_treeview(
      const hmap::Vec2<float> node_position = {0.f, 0.f});

  void render_node_editor();

  void render_node_list();

  void render_settings(std::string node_id);

  void render_view_node(std::string node_id);

  void render_view_nodes();

  void view_node_layout();

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;

  std::map<int, Link> links = {};
  int                 id_counter = 0;

  ImNodesEditorContext    *p_node_editor_context = nullptr;
  bool                     link_has_been_dropped = false;
  std::vector<int>         selected_node_hash_ids = {};
  std::vector<std::string> selected_node_ids = {};

  bool open_node_list_window = false;
  bool show_settings = true;
};

} // namespace hesiod::vnode
