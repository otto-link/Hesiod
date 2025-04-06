/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/graph_node.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/broadcast_node.hpp"
#include "hesiod/model/nodes/node_factory.hpp"

#include <iostream>

namespace hesiod
{

GraphNode::GraphNode(const std::string &id, std::shared_ptr<ModelConfig> config)
    : gnode::Graph(id), hmap::Terrain(), config(config)
{
  LOG->trace("GraphNode::GraphNode");
  this->config->log_debug();
}

void GraphNode::json_from(nlohmann::json const &json, bool override_config)
{
  LOG->trace("GraphNode::json_from, graph {}", this->get_id());

  // if set to false, the actual state of the configuration is used
  // (can for instance be used when modifying the configuration of an
  // existing graph)
  if (override_config)
    this->config->json_from(json["model_config"]);

  // hmap::Terrain
  auto vo = json["origin"].get<std::vector<float>>();
  auto vs = json["size"].get<std::vector<float>>();

  this->set_origin(hmap::Vec2<float>(vo[0], vo[1]));
  this->set_size(hmap::Vec2<float>(vs[0], vs[1]));
  this->set_rotation_angle(json["rotation_angle"]);

  // populate nodes
  for (auto &json_node : json["nodes"])
  {
    LOG->trace("GraphNode::json_from, node type: {}",
               json_node["label"].get<std::string>());

    // instanciate the node
    std::string                  node_type = json_node["label"];
    std::shared_ptr<gnode::Node> node = node_factory(node_type, this->config);

    this->add_node(node, json_node["id"].get<std::string>());

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
  LOG->trace("GraphNode::json_to, graph {}", this->get_id());

  nlohmann::json json;
  json["id"] = this->get_id();
  json["id_count"] = this->get_id_count();
  json["model_config"] = this->config->json_to();

  // hmap::Terrain
  json["origin"] = {this->get_origin().x, this->get_origin().y};
  json["size"] = {this->get_size().x, this->get_size().y};
  json["rotation_angle"] = this->get_rotation_angle();

  // nodes
  json["nodes"] = nlohmann::json::array();

  for (auto &[key, p_node] : this->nodes)
  {
    nlohmann::json json_node;
    json_node = dynamic_cast<BaseNode *>(p_node.get())->json_to();
    json["nodes"].push_back(json_node);
  }

  // links
  json["links"] = nlohmann::json::array();

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

  return json;
}

std::string GraphNode::new_node(const std::string &node_type)
{
  std::shared_ptr<gnode::Node> node = node_factory(node_type, this->config);
  node->compute();

  std::string node_id = this->add_node(node);

  // for the Broadcast nodes, generate their broascasting ID
  gnode::Node *p_node = this->get_node_ref_by_id(node_id);

  if (p_node->get_label() == "Broadcast")
  {
    BroadcastNode *p_broadcast_node = dynamic_cast<BroadcastNode *>(p_node);
    p_broadcast_node->generate_broadcast_tag();
  }

  return node_id;
}

} // namespace hesiod
