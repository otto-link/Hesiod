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

#include "hesiod/model/broadcast_param.hpp"
#include "hesiod/model/graph_node.hpp"
#include "hesiod/model/model_config.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

struct BroadcastParam; // forward

// =====================================
// GraphEditor
// =====================================
class GraphEditor : public QObject, public GraphNode
{
  Q_OBJECT

public:
  GraphEditor() = default;
  GraphEditor(const std::string           &id,
              std::shared_ptr<ModelConfig> config,
              bool                         headless = false);

  // --- Serialization ---
  void           json_from(nlohmann::json const &json, bool override_config = true);
  nlohmann::json json_to() const;

  void clear();

  // --- Inter-graph Data Broadcasting ---
  BroadcastMap *get_p_broadcast_params() { return this->p_broadcast_params; }
  void          setup_broadcast_receive_node(const std::string &node_id);
  void          set_p_broadcast_params(BroadcastMap *new_p_broadcast_params);

  // --- GUI Accessor ---
  gngui::GraphViewer *get_p_viewer() { return this->viewer.get(); }

  // --- GNode::Graph Update Override ---
  void update() override;
  void update(std::string id) override;

signals:
  // --- Graph Computation Signals ---
  void broadcast_node_updated(const std::string &graph_id, const std::string &tag);
  void has_been_updated(const std::string &graph_id);
  void node_compute_finished(const std::string &id);
  void request_update_receive_nodes_tag_list();

  // --- User Actions Signals ---
  void new_broadcast_tag(const std::string     &tag,
                         const hmap::Terrain   *t_source,
                         const hmap::Heightmap *h_source);
  void new_node_created(const std::string &graph_id, const std::string &id);
  void node_deleted(const std::string &graph_id, const std::string &id);
  void remove_broadcast_tag(const std::string &tag);

public slots:
  // --- Graph Computation Events ---
  void on_broadcast_node_updated(const std::string &tag);

  // --- User Actions ---
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

  // --- GUI Helpers ---
  void graph_viewer_disable();
  void graph_viewer_enable();

  // --- Members ---
  std::unique_ptr<gngui::GraphViewer> viewer = std::unique_ptr<gngui::GraphViewer>(
      nullptr);

  std::vector<std::unique_ptr<QWidget>> data_viewers;

  nlohmann::json json_copy_buffer;
  bool           update_node_on_new_link = true;
  BroadcastMap  *p_broadcast_params = nullptr; // own by GraphManager
};

} // namespace hesiod