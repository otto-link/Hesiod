/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QObject>

#include "nlohmann/json.hpp"

#include "gnode/graph.hpp"
#include "highmap/coord_frame.hpp"

#include "hesiod/model/broadcast_param.hpp"
#include "hesiod/model/graph_config.hpp"

namespace hesiod
{

class BaseNode; // forward

// =====================================
// GraphNode
// =====================================
class GraphNode : public QObject, public gnode::Graph, public hmap::CoordFrame
{
  Q_OBJECT

public:
  GraphNode() = delete;
  GraphNode(const std::string &id, const std::shared_ptr<GraphConfig> &config);

  // --- Serialization ---
  void json_from(nlohmann::json const &json, GraphConfig *p_input_config = nullptr);
  nlohmann::json json_to() const;

  // --- Config. ---
  GraphConfig *get_config_ref() { return this->config.get(); }

  // --- Node Factory (create nodes from their type) ---
  std::string add_node(const std::string &node_type);

  // --- GNode::Graph override ---
  std::string  add_node(const std::shared_ptr<gnode::Node> &p_node,
                        const std::string                  &id = "") override;
  virtual void remove_node(const std::string &id) override;
  void         update() override;
  void         update(const std::string &node_id) override;

  // --- Inter-graph Broadcasting ---
  BroadcastMap *get_p_broadcast_params() { return this->p_broadcast_params; }
  void          set_p_broadcast_params(BroadcastMap *new_p_broadcast_params);
  void          on_broadcast_node_updated(const std::string &tag);

  // --- Others... ---
  void reseed(bool backward);

signals:
  // --- Signals ---
  void compute_started(const std::string &graph_id, const std::string &node_id);
  void compute_finished(const std::string &graph_id, const std::string &node_id);
  void update_started(const std::string &graph_id);
  void update_finished(const std::string &graph_id);

  void broadcast_node_updated(const std::string &graph_id, const std::string &tag);
  void new_broadcast_tag(const std::string      &tag,
                         const hmap::CoordFrame *t_source,
                         const hmap::Heightmap  *h_source);
  void remove_broadcast_tag(const std::string &tag);

private:
  // --- Helpers ---
  void setup_new_broadcast_node(BaseNode *p_node);
  void setup_new_receive_node(BaseNode *p_node);

  // --- Members ---
  std::shared_ptr<GraphConfig> config;
  BroadcastMap                *p_broadcast_params = nullptr; // own by GraphManager
};

} // namespace hesiod
