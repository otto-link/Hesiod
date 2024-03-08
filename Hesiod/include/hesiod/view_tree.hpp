/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <GL/glut.h>
#include <string>

#include "highmap.hpp"
#include <imgui_node_editor.h>

#include "hesiod/control_node.hpp"
#include "hesiod/serialization.hpp"
#include "hesiod/view_node.hpp"

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

  SERIALIZATION_V2_IMPLEMENT_BASE();
};

enum class ViewTreesSerializationType
{
  PLAIN,
  BJDATA,
  BSON,
  CBOR,
  MESSAGEPACK,
  UBJSON
};

class ViewTree : public gnode::Tree, public serialization::SerializationBase
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

  std::string get_node_type(std::string node_id) const;

  ax::NodeEditor::EditorContext *get_p_node_editor_context() const;

  void set_sto(hmap::Vec2<int> new_shape,
               hmap::Vec2<int> new_tiling,
               float           new_overlap);

  void set_viewer_node_id(std::string node_id);

  std::string add_view_node(std::string control_node_type,
                            std::string node_id = "");

  void automatic_node_layout();

  void clear();

  void export_view3d(std::string fname);

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

  void update_image_texture_view3d(bool vertex_array_update = true);

  void update_view3d_basemesh();

  // used for serialization now
  // see class ControlNodeInstancing

  inline hmap::Vec2<int> get_shape()
  {
    return this->shape;
  }

  inline hmap::Vec2<int> get_tiling()
  {
    return this->tiling;
  }

  inline float get_overlap()
  {
    return this->overlap;
  }

  // serialization

  void load_state(std::string fname, ViewTreesSerializationType stype);

  void save_state(std::string fname, ViewTreesSerializationType stype);

  SERIALIZATION_V2_IMPLEMENT_BASE();

private:
  hmap::Vec2<int> shape;
  hmap::Vec2<int> tiling;
  float           overlap;

  std::map<int, Link> links = {};
  int                 id_counter = 0;

  std::string json_filename = "";

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

  std::map<std::string, int> cmap_map = {
      {"gray", hmap::cmap::gray},
      {"inferno", hmap::cmap::inferno},
      {"nipy_spectral", hmap::cmap::nipy_spectral},
      {"terrain", hmap::cmap::terrain}};

  // 3D viewer
  bool                 open_view3d_window = false;
  bool                 show_view3d_on_background = false;
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
  float h_scale = 0.4f;
  float alpha_x = 35.f;
  float alpha_y = -25.f;
  float delta_x = 0.f;
  float delta_y = 0.f;
  bool  wireframe = false;
  bool  auto_rotate = false;

  bool                     show_settings = false;
  ax::NodeEditor::NodeId   context_menu_node_hid;
  std::vector<std::string> key_sort;
  ViewTreesSerializationType serialization_type;
};

// HELPERS
std::string node_type_from_id(std::string node_id);

} // namespace hesiod::vnode
