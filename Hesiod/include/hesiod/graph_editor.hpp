/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file graph_editor.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <memory>

#include <QObject>

#include "gnodegui/graph_viewer.hpp"

#include "hesiod/model/graph_node.hpp"
#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class GraphEditor : public QObject, public GraphNode
{
  Q_OBJECT
public:
  GraphEditor() = default;

  GraphEditor(const std::string           &id,
              std::shared_ptr<ModelConfig> config,
              bool                         headless = false);

  gngui::GraphViewer *get_p_viewer() { return this->viewer.get(); }

  void json_from(nlohmann::json const &json);

  nlohmann::json json_to() const;

public Q_SLOTS:
  void on_connection_deleted(const std::string &id_out,
                             const std::string &port_id_out,
                             const std::string &id_in,
                             const std::string &port_id_in);

  void on_connection_finished(const std::string &id_out,
                              const std::string &port_id_out,
                              const std::string &id_in,
                              const std::string &port_id_in);

  void on_graph_clear_request();

  void on_graph_load_request();

  void on_graph_reload_request();

  void on_graph_save_as_request();

  void on_graph_save_request();

  // for serialization only
  void on_new_graphics_node_request(const std::string &node_id, QPointF scene_pos);

  void on_new_node_request(const std::string &node_type, QPointF scene_pos);

  void on_node_deleted_request(const std::string &node_id);

  void on_node_reload_request(const std::string &node_id);

  void on_node_right_clicked(const std::string &node_id, QPointF scene_pos);

  void on_viewport_request();

private:
  std::unique_ptr<gngui::GraphViewer> viewer = std::unique_ptr<gngui::GraphViewer>(
      nullptr);

  std::filesystem::path fname = "";
};

} // namespace hesiod