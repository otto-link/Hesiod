/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <string>

#include <GL/glut.h>

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

  ImU32 get_node_color(std::string node_id);

  std::string get_node_type(std::string node_id);

  ax::NodeEditor::EditorContext *get_p_node_editor_context() const;

  hesiod::vnode::ViewControlNode *get_view_control_node_ref_by_id(
      std::string node_id) const;

  void set_viewer_node_id(std::string node_id);

  std::string add_view_node(std::string control_node_type,
                            std::string node_id = "");

  void automatic_node_layout();

  void insert_clone_node(std::string node_id);

  void new_link(int port_hash_id_from, int port_hash_id_to);

  void new_link(std::string node_id_from, // out
                std::string port_id_from,
                std::string node_id_to, // in
                std::string port_id_to);

  void post_update();

  void remove_link(int link_id);

  void remove_view_node(std::string node_id);

  void render_links();

  std::string render_new_node_popup();

  std::string render_new_node_treeview(const ImVec2 node_position = {0.f, 0.f});

  void render_node_editor();

  void render_node_list();

  void render_settings(std::string node_id);

  void render_view_node(std::string node_id);

  void render_view_nodes();

  void render_view2d();

  void render_view3d();

  void update_image_texture_view2d();

  void update_image_texture_view3d(bool only_matrix_update = false);

  void update_view3d_basemesh();

  // serialization

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

  bool open_node_list_window = false;

  std::string viewer_node_id = "";

  // 2D viewer
  bool            open_view2d_window = false;
  GLuint          image_texture_view2d;
  hmap::Vec2<int> shape_view2d = {512, 512};
  int             cmap_view2d = hmap::cmap::inferno;
  bool            hillshade_view2d = false;
  float           view2d_zoom = 100.f;
  ImVec2          view2d_uv0 = {0.f, 0.f};

  std::map<std::string, int> cmap_map = {{"gray", hmap::cmap::gray},
                                         {"inferno", hmap::cmap::inferno},
                                         {"terrain", hmap::cmap::terrain}};

  // 3D viewer
  bool                 open_view3d_window = false;
  GLuint               image_texture_view3d;
  GLuint               shader_id;
  GLuint               vertex_array_id;
  GLuint               vertex_buffer;
  GLuint               color_buffer;
  GLuint               FBO;
  GLuint               RBO;
  hmap::Vec2<int>      shape_view3d = {512, 512};
  std::vector<GLfloat> vertices = {};
  std::vector<GLfloat> colors = {};

  float scale = 0.7f;
  float h_scale = 0.2f;
  float alpha_x = 0.f;
  float alpha_y = 0.f;
  float delta_x = 0.f;
  float delta_y = 0.f;
  bool  wireframe = false;
  bool  auto_rotate = false;

  bool                     show_settings = false;
  ax::NodeEditor::NodeId   context_menu_node_hid;
  std::vector<std::string> key_sort;
};

// HELPERS
std::string node_type_from_id(std::string node_id);

} // namespace hesiod::vnode
