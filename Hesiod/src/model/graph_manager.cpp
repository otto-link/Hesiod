/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "nlohmann/json.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/graph_manager.hpp"
#include "hesiod/model/graph_node.hpp"
#include "hesiod/model/model_config.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

GraphManager::GraphManager(const std::string &id) : id(id)
{
  LOG->trace("GraphManager::GraphManager: id: {}", id);
}

std::string GraphManager::add_graph_node(const std::shared_ptr<GraphNode> &p_graph_node,
                                         const std::string                &id)
{
  LOG->trace("GraphManager::add_graph_node: {}", id);

  std::string graph_id = id;

  // use node pointer as ID if none is provided
  if (id.empty())
    graph_id = std::to_string(this->id_count++);

  // check if the ID is available
  if (!this->is_graph_id_available(graph_id))
    throw std::runtime_error("Graph ID already used: " + graph_id);

  // add the graph to the map and store the ID within the graph (in case of)
  this->graph_nodes[graph_id] = p_graph_node;
  p_graph_node->set_id(graph_id);

  // store a reference to the global storage of broadcasting data
  p_graph_node->set_p_broadcast_params(&broadcast_params);

  this->graph_order.push_back(graph_id);

  // connections for broadcasting data between graph_nodes
  this->connect(p_graph_node.get(),
                &GraphNode::broadcast_node_updated,
                this,
                &GraphManager::on_broadcast_node_updated);

  this->connect(p_graph_node.get(),
                &GraphNode::new_broadcast_tag,
                this,
                &GraphManager::on_new_broadcast_tag);

  this->connect(p_graph_node.get(),
                &GraphNode::remove_broadcast_tag,
                this,
                &GraphManager::on_remove_broadcast_tag);

  // // NOTE GUI
  // this->connect(p_graph_node.get(),
  //               &GraphNode::request_update_receive_nodes_tag_list,
  //               this,
  //               &GraphManager::update_receive_nodes_tag_list); // TODO add

  return graph_id;
}

void GraphManager::clear()
{
  LOG->trace("GraphManager::clear");

  for (auto &[id, graph] : this->graph_nodes)
    graph->clear();

  this->id_count = 0;
  this->graph_nodes.clear();
  this->graph_order.clear();
  this->broadcast_params.clear();
}

const BroadcastMap &GraphManager::get_broadcast_params()
{
  return this->broadcast_params;
}

const GraphNodeMap &GraphManager::get_graph_nodes() { return this->graph_nodes; }

const std::vector<std::string> &GraphManager::get_graph_order()
{
  return this->graph_order;
}

int GraphManager::get_graph_order_index(const std::string &id)
{
  if (this->is_graph_id_available(id))
  {
    LOG->critical("GraphManager::get_graph_order_index, graph ID not known: {}", id);
    throw std::runtime_error("Graph ID not known: " + id);
  }

  auto itr = std::find(this->graph_order.begin(), this->graph_order.end(), id);
  return (int)std::distance(this->graph_order.begin(), itr);
}

GraphNode *GraphManager::get_graph_ref_by_id(const std::string &id)
{
  auto it = graph_nodes.find(id);
  if (it == graph_nodes.end())
    return nullptr;
  else
    return it->second.get();
}

std::string GraphManager::get_id() const { return this->id; }

bool GraphManager::is_graph_above(const std::string &graph_id,
                                  const std::string &ref_graph_id)
{
  for (auto &id : this->graph_order)
  {
    if (id == graph_id)
      return false;
    else if (id == ref_graph_id)
      return true;
  }

  return false;
}

bool GraphManager::is_graph_id_available(const std::string &id)
{
  return !this->graph_nodes.contains(id);
}

void GraphManager::json_from(nlohmann::json const &json, ModelConfig *p_config)
{
  LOG->trace("GraphManager::json_from");

  // clean-up current state
  this->clear();

  // keep deserializing
  this->id_count = json["id_count"];

  // graph order
  std::vector<std::string> gid_order = json["graph_order"];

  for (auto &id : gid_order)
  {
    LOG->trace("graph id: {}", id);

    // dummy default config that will be overriden after by the
    // GraphNode instances during their 'json_from' deserialization
    auto config = std::make_shared<hesiod::ModelConfig>();
    auto graph = std::make_shared<hesiod::GraphNode>("", config);

    // in this order (add, then deserialize), add the graph node to
    // the graph manager and then deserialize the graph node because
    // the Receive nodes, if any, need a reference to the
    // broadcast_params of the GraphManager, which is provided to the
    // graph node when added...
    this->add_graph_node(graph, id);
    graph->json_from(json["graph_nodes"][id], p_config);
  }
}

nlohmann::json GraphManager::json_to() const
{
  LOG->trace("GraphManager::json_to");

  nlohmann::json json;

  // NB - broadcast_params is not serialized since it is regenerated
  // when re-creating the graph nodes
  json["id_count"] = this->id_count;
  json["graph_order"] = this->graph_order;

  // graphs
  nlohmann::json json_graphs;
  for (auto &[id, graph] : this->graph_nodes)
    json_graphs[id] = graph->json_to();

  json["graph_nodes"] = json_graphs;

  return json;
}

void GraphManager::load_from_file(const std::string &fname, ModelConfig *p_config)
{
  LOG->trace("GraphManager::load_from_file: fname {}", fname);

  nlohmann::json json = json_from_file(fname);
  this->json_from(json["graph_manager"], p_config);

  // update graphs
  this->update();
}

void GraphManager::on_broadcast_node_updated(const std::string &graph_id,
                                             const std::string &tag)
{
  LOG->trace("GraphManager::on_broadcast_node_updated: broadcasting {}", tag);

  for (auto &[gid, graph] : this->graph_nodes)
    if (true) // gid != graph_id)
    {
      // prevent any broadcast from a top layer to a sublayer, this
      // could lead to endless loop in case of cross-broadcast
      if (this->is_graph_above(gid, graph_id))
        graph->on_broadcast_node_updated(tag);
    }
}

void GraphManager::on_new_broadcast_tag(const std::string     &tag,
                                        const hmap::Terrain   *t_source,
                                        const hmap::Heightmap *h_source)
{
  LOG->trace("GraphManager::on_new_broadcast_tag: tag {}", tag);

  this->broadcast_params[tag] = BroadcastParam(t_source, h_source);
  Q_EMIT this->new_broadcast_tag(tag);
}

void GraphManager::on_remove_broadcast_tag(const std::string &tag)
{
  LOG->trace("GraphManager::on_remove_broadcast_tag: tag {}", tag);

  this->broadcast_params.erase(tag);
  Q_EMIT this->remove_broadcast_tag(tag);
}

void GraphManager::remove_graph_node(const std::string &id)
{
  LOG->trace("GraphManager::remove_graph_node: id {}", id);

  if (this->is_graph_id_available(id))
    return;

  this->graph_order.erase(
      std::remove(this->graph_order.begin(), this->graph_order.end(), id),
      this->graph_order.end());

  this->graph_nodes.erase(id);
}

void GraphManager::save_to_file(const std::string &fname) const
{
  LOG->trace("GraphManager::save_to_file: fname {}", fname);

  // fill-in json with graph data
  nlohmann::json json;
  json["graph_manager"] = this->json_to();
  json_to_file(json, fname);
}

void GraphManager::set_graph_order(const std::vector<std::string> &new_graph_order)
{
  this->graph_order = new_graph_order;
}

void GraphManager::set_id(const std::string &new_id) { this->id = new_id; }

void GraphManager::update()
{
  LOG->trace("GraphManager::update()");

  for (auto &graph_id : this->graph_order)
    this->graph_nodes.at(graph_id)->update();
}

} // namespace hesiod
