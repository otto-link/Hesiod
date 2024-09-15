/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/graph_node.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/node_factory.hpp"

#include <iostream>

namespace hesiod
{

GraphNode::GraphNode(const std::string &id, std::shared_ptr<ModelConfig> config)
    : gnode::Graph(), id(id), config(config)
{
  HLOG->trace("GraphNode::GraphNode");
  this->config->log_debug();
}

void GraphNode::json_from(nlohmann::json const &json)
{
  HLOG->trace("GraphNode::json_from, graph {}", this->get_id());

  this->clear();
  this->set_id(json["id"]);
  this->set_id_count(json["id_count"]);

  // populate nodes
  for (auto &json_node : json["nodes"])
  {
    HLOG->trace("GraphNode::json_from, node type: {}", json_node["label"]);

    // instanciate the node
    std::string                  node_type = json_node["label"];
    std::shared_ptr<gnode::Node> node = node_factory(node_type, this->config);
    this->add_node(node, json_node["id"]);

    // set its parameters
    dynamic_cast<BaseNode *>(node.get())->json_from(json_node);
  }

  // links
  for (auto &json_link : json["links"])
    this->new_link(json_link["node_id_from"].get<std::string>(),
                   json_link["port_id_from"].get<std::string>(),
                   json_link["node_id_to"].get<std::string>(),
                   json_link["port_id_to"].get<std::string>());
}

nlohmann::json GraphNode::json_to() const
{
  HLOG->trace("GraphNode::json_to, graph {}", this->get_id());

  nlohmann::json json;
  json["id"] = this->get_id();
  json["id_count"] = this->get_id_count();

  // nodes
  for (auto &[key, p_node] : this->nodes)
  {
    nlohmann::json json_node;
    json_node = dynamic_cast<BaseNode *>(p_node.get())->json_to();
    json["nodes"].push_back(json_node);
  }

  // links
  for (auto &link : this->links)
  {
    nlohmann::json json_link;

    gnode::Node *p_from = this->get_node_ref_by_id(link.from);
    gnode::Node *p_to = this->get_node_ref_by_id(link.to);

    json_link["node_id_from"] = link.from;
    json_link["node_id_to"] = link.to;
    json_link["port_id_from"] = p_from->get_port_label(link.port_from);
    json_link["port_id_to"] = p_to->get_port_label(link.port_to);

    json["links"].push_back(json_link);
  }

  std::cout << json.dump(4) << std::endl;

  return json;
}

std::string GraphNode::new_node(const std::string &node_type)
{
  std::shared_ptr<gnode::Node> node = node_factory(node_type, this->config);
  std::string                  node_id = this->add_node(node);
  return node_id;
}

void GraphNode::on_connection_deleted_request(const std::string &id_out,
                                              const std::string &port_id_out,
                                              const std::string &to_in,
                                              const std::string &port_id_in)
{
  HLOG->trace("GraphNode::on_connection_deleted_request, {}:{} -> {}:{}",
              id_out,
              port_id_out,
              to_in,
              port_id_in);

  this->remove_link(id_out, port_id_out, to_in, port_id_in);
}

void GraphNode::on_connection_finished_request(const std::string &id_out,
                                               const std::string &port_id_out,
                                               const std::string &to_in,
                                               const std::string &port_id_in)
{
  HLOG->trace("GraphNode::on_connection_finished_request, {}:{} -> {}:{}",
              id_out,
              port_id_out,
              to_in,
              port_id_in);

  this->new_link(id_out, port_id_out, to_in, port_id_in);
}

void GraphNode::on_graph_clear_request()
{
  HLOG->trace("GraphNode::on_graph_clear_request, graph {}", this->get_id());
  this->clear();
}

void GraphNode::on_graph_update_request()
{
  HLOG->trace("GraphNode::on_graph_update_request, graph {}", this->get_id());
  this->update();
}

void GraphNode::on_new_node_request(const std::string &node_type)
{
  HLOG->trace("GraphNode::on_new_node_request, node_type {}", node_type);
  this->new_node(node_type);
  // TODO propagate back to viewer?
}

void GraphNode::on_node_deleted_request(const std::string &node_id)
{
  HLOG->trace("GraphNode::on_node_deleted_request, node {}", node_id);
  this->remove_node(node_id);
}

void GraphNode::on_node_update_request(const std::string &node_id)
{
  HLOG->trace("GraphNode::on_node_update_request, node {}", node_id);
  this->update(id);
};

} // namespace hesiod
