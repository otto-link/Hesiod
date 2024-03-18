/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <filesystem>
#include <string>

#include "highmap.hpp"
#include "imgui.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <imgui_node_editor.h>

#include "hesiod/control_tree.hpp"
#include "hesiod/serialization.hpp"
#include "hesiod/shortcuts.hpp"

namespace hesiod::vnode
{

/**
 * @brief LinkInfos class, use to store informations about the data link between
 * the nodes.
 *
 * The base framework GNode does not provide an inventory of the
 * links since the data connection informations between the nodes are carried
 * out by the nodes themselve. In order to render the links, it nonetheless
 * necessary to have an 'overview' of the links (to avoid parsing the whole to
 * rebuild the links invetory).
 *
 * 'from' and 'to' depends on how the link is created and is not related to a
 * specific link direction (different from 'output' or 'input').
 */
struct LinkInfos
{
  // --- GNode (and thus Hesiod) id system, and 'hash_id' for an id system
  // compliant with the node editor

  /**
   * @brief Starting node id (GNode id system).
   */
  std::string node_id_from;

  /**
   * @brief Starting port id (GNode id system).
   */
  std::string port_id_from;

  /**
   * @brief  Starting port id (node editor id system).
   */
  int port_hash_id_from;

  /**
   * @brief Ending node id (GNode id system).
   */
  std::string node_id_to;

  /**
   * @brief Ending port id (GNode id system).
   */
  std::string port_id_to;

  /**
   * @brief  Ending port id (node editor id system).
   */
  int port_hash_id_to;

  /**
   * @brief Constructor
   */
  LinkInfos();

  LinkInfos(std::string node_id_from,
            std::string port_id_from,
            int         port_hash_id_from,
            std::string node_id_to,
            std::string port_id_to,
            int         port_hash_id_to); ///< @overload

  SERIALIZATION_V2_IMPLEMENT_BASE();
};

class ViewTree : public hesiod::cnode::ControlTree
{
public:
  bool show_comments = false;

  ViewTree(std::string     id,
           hmap::Vec2<int> shape,
           hmap::Vec2<int> tiling,
           float           overlap)
      : hesiod::cnode::ControlTree(id, shape, tiling, overlap)
  {
  }

  std::map<int, LinkInfos> get_links_infos()
  {
    return this->links_infos;
  }

  LinkInfos *get_link_ref_by_id(int link_id);

  ImU32 get_node_color(std::string node_id) const;

  std::vector<ax::NodeEditor::NodeId> get_selected_node_hid() const
  {
    return this->selected_node_hid;
  }

  void set_sto(hmap::Vec2<int> new_shape,
               hmap::Vec2<int> new_tiling,
               float           new_overlap);

  void set_selected_node_hid(
      std::vector<ax::NodeEditor::NodeId> new_selected_node_hid);

  std::string add_view_node(std::string control_node_type,
                            std::string node_id = "");

  virtual void clear() override;

  void new_link(int port_hash_id_from, int port_hash_id_to);

  void new_link(std::string node_id_from, // out
                std::string port_id_from,
                std::string node_id_to, // in
                std::string port_id_to);

  void post_update();

  void remove_link(int link_id);

  void remove_view_node(std::string node_id);

  void render_settings(std::string node_id); // stays here

  std::map<std::string, ImVec2> get_node_positions() const
  {
    return this->node_positions;
  }

  void store_node_positions(
      ax::NodeEditor::EditorContext *p_node_editor_context);

  // --- serialization

  void load_state(std::string fname);

  void save_state(std::string fname);

  SERIALIZATION_V2_IMPLEMENT_BASE();

public:
  /**
   * @brief List of callbacks function to be called after the tree has been
   * updated. Can be useful to update the content of a window for instance.
   */
  std::map<std::string, std::function<void()>> post_update_callbacks = {};

private:
  /**
   * @brief Storage of the link informations.
   */
  std::map<int, LinkInfos> links_infos = {};

  //  keep a copy of some data from the node editor(s) for conveniency and
  //  overall coherency between the node editors

  /**
   * @brief Copy the last known node selection from the node editor.
   */
  std::vector<ax::NodeEditor::NodeId> selected_node_hid = {};

  /**
   * @brief Node positions, stored in order to diffuse them to all the node
   * editors, and also used for serialization.
   */
  std::map<std::string, ImVec2> node_positions = {};
};

} // namespace hesiod::vnode
