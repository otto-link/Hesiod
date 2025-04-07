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

#include "gnodegui/graph_viewer.hpp"

#include "hesiod/model/broadcast_param.hpp"
#include "hesiod/model/graph_node.hpp"
#include "hesiod/model/model_config.hpp"

namespace hesiod
{

struct BroadcastParam; // forward

// =====================================
// GraphEditor
// =====================================
class GraphEditor : public GraphNode
{
  Q_OBJECT

public:
  GraphEditor() = default;
  GraphEditor(const std::string           &id,
              std::shared_ptr<ModelConfig> config,
              bool                         headless = false);

  void clear(); // DONE

  // --- Serialization ---
  void           json_from(nlohmann::json const &json, bool override_config = true);
  nlohmann::json json_to() const;

  // --- GUI Accessor ---
  gngui::GraphViewer *get_p_viewer() { return this->viewer.get(); } // OK

signals:
  // --- Graph Computation Signals ---
  void request_update_receive_nodes_tag_list(); // GUI

  // --- User Actions Signals ---
  void new_node_created(const std::string &graph_id, const std::string &id); // DONE
  void node_deleted(const std::string &graph_id, const std::string &id);     // GUI

public slots:
  // --- Graph Computation Events ---

  // --- User Actions ---
  void on_connection_deleted(const std::string &id_out,
                             const std::string &port_id_out,
                             const std::string &id_in,
                             const std::string &port_id_in); // DONE
  void on_connection_finished(const std::string &id_out,
                              const std::string &port_id_out,
                              const std::string &id_in,
                              const std::string &port_id_in); // DONE

  void on_graph_clear_request();    // DONE
  void on_graph_new_request();      // DONE
  void on_graph_reload_request();   // DONE
  void on_graph_settings_request(); // DONE

  void on_new_node_request(const std::string &node_type,
                           QPointF            scene_pos,
                           std::string       *p_new_node_id = nullptr);            // DONE
  void on_node_deleted_request(const std::string &node_id);                  // DONE
  void on_node_reload_request(const std::string &node_id);                   // DONE
  void on_node_right_clicked(const std::string &node_id, QPointF scene_pos); // DONE

  void on_nodes_copy_request(const std::vector<std::string> &id_list,
                             const std::vector<QPointF>     &scene_pos_list); // DONE
  void on_nodes_duplicate_request(const std::vector<std::string> &id_list,
                                  const std::vector<QPointF>     &scene_pos_list); // DONE
  void on_nodes_paste_request();                                               // DONE

  void on_viewport_request(); // DONE

  // --- Others... ---
  void on_new_graphics_node_request(const std::string &node_id,
                                    QPointF            scene_pos); // DONE

private:
  // --- Members ---
  std::unique_ptr<gngui::GraphViewer> viewer = std::unique_ptr<gngui::GraphViewer>(
      nullptr);

  std::vector<std::unique_ptr<QWidget>> data_viewers;

  nlohmann::json json_copy_buffer;
  bool           update_node_on_connection_finished = true;

  GraphNode *p_graph_node = nullptr;
};

} // namespace hesiod