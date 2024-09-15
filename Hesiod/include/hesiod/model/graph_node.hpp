/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file graph_node.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <QObject>

#include "nlohmann/json.hpp"

#include "gnode/graph.hpp"

#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class GraphNode : public QObject, public gnode::Graph
{
  Q_OBJECT
public:
  GraphNode() = delete;

  GraphNode(const std::string &id, std::shared_ptr<ModelConfig> config);

  std::string get_id() const { return this->id; }

  void json_from(nlohmann::json const &json);

  nlohmann::json json_to() const;

  std::string new_node(const std::string &node_type);

  void set_id(const std::string &new_id) { this->id = new_id; }

public Q_SLOTS:
  void on_graph_clear_request();

  void on_graph_update_request();

  void on_connection_deleted_request(const std::string &id_out,
                                     const std::string &port_id_out,
                                     const std::string &to_in,
                                     const std::string &port_id_in);

  void on_connection_finished_request(const std::string &id_out,
                                      const std::string &port_id_out,
                                      const std::string &to_in,
                                      const std::string &port_id_in);

  void on_new_node_request(const std::string &node_type);

  void on_node_deleted_request(const std::string &node_id);

  void on_node_update_request(const std::string &node_id);

private:
  std::string                  id;
  std::shared_ptr<ModelConfig> config;
};

} // namespace hesiod