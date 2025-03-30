/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file graph_editor.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2023
 */
#pragma once
#include <memory>

#include <QObject>

#include "gnodegui/graph_viewer.hpp"

#include "hesiod/model/graph_node.hpp"
#include "hesiod/model/model_config.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

struct BroadcastParam; // forward

class GraphEditor : public QObject, public GraphNode
{
  Q_OBJECT

public:
  GraphEditor() = default;

  GraphEditor(const std::string           &id,
              std::shared_ptr<ModelConfig> config,
              bool                         headless = false);

  void clear();

  std::map<std::string, BroadcastParam> *get_p_broadcast_params()
  {
    return this->p_broadcast_params;
  }

  gngui::GraphViewer *get_p_viewer() { return this->viewer.get(); }

  void json_from(nlohmann::json const &json,
                 bool                  override_config = true,
                 bool                  clear_existing_content = true,
                 const std::string    &prefix_id = "");

  nlohmann::json json_to() const;

  void set_p_broadcast_params(
      std::map<std::string, BroadcastParam> *new_p_broadcast_params)
  {
    this->p_broadcast_params = new_p_broadcast_params;
  }

  void update(); // GNode::Graph

  void update(std::string id); // GNode::Graph

Q_SIGNALS:
  void broadcast_node_updated(const std::string &graph_id, const std::string &tag);

  void node_compute_finished(const std::string &id);

  void new_broadcast_tag(const std::string     &tag,
                         const hmap::Terrain   *t_source,
                         const hmap::Heightmap *h_source);

  void remove_broadcast_tag(const std::string &tag);

  void request_update_receive_nodes_tag_list();

public Q_SLOTS:
  void on_broadcast_node_updated(const std::string &tag);

  void on_connection_deleted(const std::string &id_out,
                             const std::string &port_id_out,
                             const std::string &id_in,
                             const std::string &port_id_in);

  void on_connection_finished(const std::string &id_out,
                              const std::string &port_id_out,
                              const std::string &id_in,
                              const std::string &port_id_in);

  void on_graph_clear_request();

  void on_graph_new_request();

  void on_graph_reload_request();

  void on_graph_settings_request();

  void on_new_graphics_node_request(const std::string &node_id, QPointF scene_pos);

  void on_new_node_request(const std::string &node_type,
                           QPointF            scene_pos,
                           std::string       *p_new_node_id = nullptr);

  void on_node_deleted_request(const std::string &node_id);

  void on_node_reload_request(const std::string &node_id);

  void on_node_right_clicked(const std::string &node_id, QPointF scene_pos);

  void on_nodes_copy_request(const std::vector<std::string> &id_list,
                             const std::vector<QPointF>     &scene_pos_list);

  void on_nodes_duplicate_request(const std::vector<std::string> &id_list,
                                  const std::vector<QPointF>     &scene_pos_list);

  void on_nodes_paste_request();

  void on_viewport_request();

private:
  void connect_node_for_broadcasting(BaseNode *p_node);

private:
  std::unique_ptr<gngui::GraphViewer> viewer = std::unique_ptr<gngui::GraphViewer>(
      nullptr);

  nlohmann::json json_copy_buffer;

  std::vector<std::unique_ptr<QWidget>> data_viewers;

  bool update_node_on_new_link = true;

  void graph_viewer_disable();

  void graph_viewer_enable();

  // ownership by GraphManager
  std::map<std::string, BroadcastParam> *p_broadcast_params = nullptr;
};

} // namespace hesiod