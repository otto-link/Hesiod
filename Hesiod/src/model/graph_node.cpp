/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/graph_node.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/broadcast_node.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/nodes/receive_node.hpp"

#include <iostream>

namespace hesiod
{

GraphNode::GraphNode(const std::string &id, const std::shared_ptr<ModelConfig> &config)
    : gnode::Graph(id), hmap::CoordFrame(), config(config)
{
  LOG->trace("GraphNode::GraphNode");
}

std::string GraphNode::add_node(const std::string &node_type)
{
  LOG->trace("GraphNode::add_node: node_type = {}", node_type);

  // this->config.reset();
  // this->config = std::make_shared<hesiod::ModelConfig>();

  if (this->config)
  {
    LOG->trace("CONFIG OK");
    this->config->log_debug();
  }
  else
    LOG->trace("CONFIG NOT OK");

  std::shared_ptr<gnode::Node> node = node_factory(node_type, this->config);
  node->compute();

  std::string node_id = this->add_node(node);

  return node_id;
}

std::string GraphNode::add_node(const std::shared_ptr<gnode::Node> &node,
                                const std::string                  &id)
{
  LOG->trace("GraphNode::add_node: id = {}", id);

  // basic GNode adding...
  std::string node_id = gnode::Graph::add_node(node, id);

  // connections
  BaseNode *p_basenode = dynamic_cast<BaseNode *>(node.get());

  this->connect(p_basenode,
                &BaseNode::compute_started,
                this,
                [this, node_id]()
                { Q_EMIT this->compute_started(this->get_id(), node_id); });

  this->connect(p_basenode,
                &BaseNode::compute_finished,
                this,
                [this, node_id]()
                { Q_EMIT this->compute_finished(this->get_id(), node_id); });

  // "special" nodes treatmentxs
  std::string node_type = p_basenode->get_node_type();

  if (node_type == "Broadcast")
    this->setup_new_broadcast_node(p_basenode);

  if (node_type == "Receive")
    this->setup_new_receive_node(p_basenode);

  return node_id;
}

void GraphNode::json_from(nlohmann::json const &json, ModelConfig *p_input_config)
{
  LOG->trace("GraphNode::json_from, graph {}", this->get_id());

  // override the current config if one is provided
  if (p_input_config)
  {
    this->config = std::make_shared<ModelConfig>(*p_input_config);

    LOG->trace("GraphNode::json_from: ModelConfig override");
    this->config->log_debug();
  }
  else
    this->config->json_from(json["model_config"]);

  this->set_id(json["id"]);
  this->set_id_count(json["id_count"]);

  // hmap::CoordFrame
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

  // hmap::CoordFrame
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

void GraphNode::on_broadcast_node_updated(const std::string &tag)
{
  LOG->trace("GraphEditor::on_broadcast_node_updated: tag: {}", tag);

  // loop over the nodes and update those with Receive type
  for (auto &[node_id, p_gnode] : this->nodes)
  {
    BaseNode *p_node = dynamic_cast<BaseNode *>(p_gnode.get());

    if (p_node->get_label() == "Receive")
    {
      ReceiveNode *p_receive_node = dynamic_cast<ReceiveNode *>(p_node);

      if (!p_receive_node)
      {
        LOG->critical("GraphEditor::on_broadcast_node_updated: could not properly recast "
                      "the node as a receiver. Tag {}",
                      tag);
        throw std::runtime_error("could not properly recast the node as a receiver");
      }
      else
      {
        // only update Receive nodes with the proper tag
        if (p_receive_node->get_current_tag() == tag)
          this->update(node_id);
      }
    }
  }
}

void GraphNode::remove_node(const std::string &id)
{
  LOG->trace("GraphNode::remove_node: id = {}", id);

  // "special" nodes treatment
  BaseNode   *p_basenode = this->get_node_ref_by_id<BaseNode>(id);
  std::string node_type = p_basenode->get_node_type();

  if (node_type == "Broadcast")
  {
    std::string tag = this->get_node_ref_by_id<BroadcastNode>(id)->get_broadcast_tag();

    LOG->trace("GraphNode::remove_node: removing broadcast tag {}", tag);

    Q_EMIT this->remove_broadcast_tag(tag);
  }

  // basic GNode removing...
  gnode::Graph::remove_node(id);
}

void GraphNode::set_p_broadcast_params(BroadcastMap *new_p_broadcast_params)
{
  LOG->trace("GraphNode::set_p_broadcast_params: ptr = {}",
             new_p_broadcast_params ? "OK" : "nullptr");

  this->p_broadcast_params = new_p_broadcast_params;
}

void GraphNode::setup_new_broadcast_node(BaseNode *p_node)
{
  LOG->trace("GraphNode::setup_new_broadcast_node");

  BroadcastNode *p_broadcast_node = dynamic_cast<BroadcastNode *>(p_node);

  if (!p_broadcast_node)
  {
    LOG->error("GraphNode::setup_new_broadcast_node: Invalid node type");
    return;
  }

  // connect
  this->connect(p_broadcast_node,
                &BroadcastNode::broadcast_node_updated,
                this,
                [this](const std::string &graph_id, const std::string &tag)
                {
                  // pass through, re-emit the signals by the graph
                  // editor (to be handled by the graph manager above)
                  LOG->trace("graph_node broadcasting, tag: {}", tag);
                  Q_EMIT this->broadcast_node_updated(graph_id, tag);
                });

  // broadcast infos: // store broadcast parameters (to be handled to the graph manage
  // above). Use an output port to store the data, to ensure it is
  // always available, full of zeros in worst case scenario
  const std::string tag = this->get_node_ref_by_id<BroadcastNode>(p_node->get_id())
                              ->get_broadcast_tag();
  const hmap::CoordFrame *t_source = dynamic_cast<hmap::CoordFrame *>(this);
  const hmap::Heightmap  *h_source = this->get_node_ref_by_id(p_node->get_id())
                                        ->get_value_ref<hmap::Heightmap>("thru");

  Q_EMIT this->new_broadcast_tag(tag, t_source, h_source);
}

void GraphNode::setup_new_receive_node(BaseNode *p_node)
{
  LOG->trace("GraphNode::setup_new_receive_node");

  ReceiveNode *p_receive_node = dynamic_cast<ReceiveNode *>(p_node);

  if (!p_receive_node)
  {
    LOG->error("GraphNode::setup_new_receive_node: Invalid node type");
    return;
  }

  p_receive_node->set_p_broadcast_params(this->p_broadcast_params);
  p_receive_node->set_p_coord_frame(dynamic_cast<hmap::CoordFrame *>(this));
}

void GraphNode::update()
{
  LOG->trace("GraphNode::update");

  Q_EMIT this->update_started(this->get_id());
  gnode::Graph::update();
  Q_EMIT this->update_finished(this->get_id());
}

void GraphNode::update(std::string id)
{
  LOG->trace("GraphNode::update: id = {}", id);

  Q_EMIT this->update_started(this->get_id());
  gnode::Graph::update(id);
  Q_EMIT this->update_finished(this->get_id());
}

} // namespace hesiod
