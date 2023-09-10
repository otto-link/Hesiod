/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include <cereal/archives/json.hpp>
#include <imgui_node_editor.h>

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

  template <class Archive> void serialize(Archive &ar)
  {
    ar(cereal::make_nvp("node_id_from", node_id_from),
       cereal::make_nvp("port_id_from", port_id_from),
       cereal::make_nvp("port_hash_id_from", port_hash_id_from),
       cereal::make_nvp("node_id_to", node_id_to),
       cereal::make_nvp("port_id_to", port_id_to),
       cereal::make_nvp("port_hash_id_to", port_hash_id_to));
  }
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

  std::string get_node_type(std::string node_id);

  ax::NodeEditor::EditorContext *get_p_node_editor_context() const;

  hesiod::vnode::ViewControlNode *get_view_control_node_ref_by_id(
      std::string node_id);

  void set_view2d_node_id(std::string node_id);

  std::string add_view_node(std::string control_node_type,
                            std::string node_id = "");

  void automatic_node_layout();

  void insert_clone_node(std::string node_id);

  void new_link(int port_hash_id_from, int port_hash_id_to);

  void new_link(std::string node_id_from, // out
                std::string port_id_from,
                std::string node_id_to, // in
                std::string port_id_to);

  void remove_link(int link_id);

  void remove_view_node(std::string node_id);

  void render_links();

  void render_new_node_popup();

  std::string render_new_node_treeview(const ImVec2 node_position = {0.f, 0.f});

  void render_node_editor();

  void render_node_list();

  void render_settings(std::string node_id);

  void render_view_node(std::string node_id);

  void render_view_nodes();

  void render_view2d(std::string node_id);

  void load_state(std::string fname);

  void save_state(std::string fname);

  template <class Archive> void load(Archive &archive);

  template <class Archive> void save(Archive &archive) const;

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;

  std::map<int, Link> links = {};
  int                 id_counter = 0;

  std::vector<ax::NodeEditor::NodeId> selected_node_hid = {};

  ax::NodeEditor::EditorContext *p_node_editor_context = nullptr;

  bool                     open_node_list_window = false;
  bool                     open_view2d_window = false;
  std::string              view2d_node_id = "";
  bool                     show_settings = false;
  ax::NodeEditor::NodeId   context_menu_node_hid;
  std::vector<std::string> key_sort;
};

// HELPERS
std::string node_type_from_id(std::string node_id);

} // namespace hesiod::vnode
