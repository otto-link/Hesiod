/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file graph_node_widget.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include "gnodegui/graph_viewer.hpp"

namespace hesiod
{

class GraphNode; // forward

// =====================================
// GraphNodeWidget
// =====================================
class GraphNodeWidget : public gngui::GraphViewer
{
  Q_OBJECT

public:
  // --- Constructor and Setup ---
  GraphNodeWidget(GraphNode *p_graph_node);

  void clear_all();
  void clear_data_viewers();
  void clear_graphic_scene();
  void setup_connections();

  // --- Serialization ---
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_import(nlohmann::json const &json, QPointF scene_pos);
  nlohmann::json json_to() const;

  GraphNode *get_p_graph_node();

signals:
  // --- User Actions Signals ---
  void has_been_cleared(const std::string &graph_id);
  void new_node_created(const std::string &graph_id, const std::string &id);
  void node_deleted(const std::string &graph_id, const std::string &id);

public slots:
  void closeEvent(QCloseEvent *event) override;

  // --- User Actions ---
  void on_connection_deleted(const std::string &id_out,
                             const std::string &port_id_out,
                             const std::string &id_in,
                             const std::string &port_id_in);
  void on_connection_dropped(const std::string &node_id,
                             const std::string &port_id,
                             QPointF            scene_pos);
  void on_connection_finished(const std::string &id_out,
                              const std::string &port_id_out,
                              const std::string &id_in,
                              const std::string &port_id_in);

  void on_graph_clear_request();
  void on_graph_new_request();
  void on_graph_reload_request();
  void on_graph_settings_request();

  std::string on_new_node_request(const std::string &node_type, QPointF scene_pos);
  void        on_node_deleted_request(const std::string &node_id);
  void        on_node_reload_request(const std::string &node_id);
  void        on_node_right_clicked(const std::string &node_id, QPointF scene_pos);

  void on_nodes_copy_request(const std::vector<std::string> &id_list,
                             const std::vector<QPointF>     &scene_pos_list);
  void on_nodes_duplicate_request(const std::vector<std::string> &id_list,
                                  const std::vector<QPointF>     &scene_pos_list);
  void on_nodes_paste_request();

  void on_viewport_request();

  // --- Others... ---
  void on_new_graphics_node_request(const std::string &node_id, QPointF scene_pos);

private:
  // --- Members ---
  GraphNode                            *p_graph_node; // own by GraphManager
  std::vector<std::unique_ptr<QWidget>> data_viewers;
  bool                                  update_node_on_connection_finished = true;
  nlohmann::json                        json_copy_buffer;
  std::string                           last_node_created_id = "";
};

} // namespace hesiod