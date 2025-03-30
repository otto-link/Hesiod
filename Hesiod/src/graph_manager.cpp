/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>
#include <iostream>

#include <QHBoxLayout>

#include "hesiod/graph_manager.hpp"
#include "hesiod/gui/main_window.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/receive_node.hpp"

namespace hesiod
{

bool GraphManager::is_graph_id_available(const std::string &id)
{
  return !this->graphs.contains(id);
}

std::string GraphManager::add_graph_editor(
    const std::shared_ptr<GraphEditor> &p_graph_editor,
    const std::string                  &id)
{
  LOG->trace("GraphManager::add_graph_editor: {}", id);

  std::string graph_id = id;

  // use node pointer as ID if none is provided
  if (id.empty())
    graph_id = std::to_string(this->id_count++);

  // check if the ID is available
  if (!this->is_graph_id_available(graph_id))
    throw std::runtime_error("Graph ID already used: " + graph_id);

  // add the graph to the map and store the ID within the graph (in case of)
  this->graphs[graph_id] = p_graph_editor;
  p_graph_editor->set_id(graph_id);

  // store a reference to the global storage of broadcasting data
  p_graph_editor->set_p_broadcast_params(&broadcast_params);

  this->graph_order.push_back(graph_id);

  // connections for broadcasting data between graphs
  this->connect(p_graph_editor.get(),
                &GraphEditor::broadcast_node_updated,
                this,
                &GraphManager::on_broadcast_node_updated);

  this->connect(p_graph_editor.get(),
                &GraphEditor::new_broadcast_tag,
                this,
                &GraphManager::on_new_broadcast_tag);

  this->connect(p_graph_editor.get(),
                &GraphEditor::remove_broadcast_tag,
                this,
                &GraphManager::on_remove_broadcast_tag);

  this->connect(p_graph_editor.get(),
                &GraphEditor::request_update_receive_nodes_tag_list,
                this,
                &GraphManager::update_receive_nodes_tag_list);

  // update GUI
  this->update_tab_widget();

  return graph_id;
}

void GraphManager::dump() const
{
  std::cout << "GraphManager::dump\n";
  for (auto &id : graph_order)
    std::cout << " - id: " << id << "\n";
}

GraphEditor *GraphManager::get_graph_ref_by_id(const std::string &id)
{
  auto it = graphs.find(id);
  if (it == graphs.end())
    return nullptr;
  else
    return it->second.get();
}

GraphEditor *GraphManager::get_graph_ref_by_id_previous(const std::string &id)
{
  auto it = std::find(this->graph_order.begin(), this->graph_order.end(), id);

  if (it != this->graph_order.begin() && it != this->graph_order.end())
    return this->graphs[*(it - 1)].get();

  return nullptr;
}

GraphEditor *GraphManager::get_graph_ref_by_id_next(const std::string &id)
{
  auto it = std::find(this->graph_order.begin(), this->graph_order.end(), id);

  if (it != this->graph_order.end() && (it + 1) != this->graph_order.end())
    return this->graphs[*(it + 1)].get();

  return nullptr;
}

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

void GraphManager::json_from(nlohmann::json const &json)
{
  LOG->trace("GraphManager::json_from");

  // TODO fix config change in batch mode

  // clean-up current state
  this->graphs.clear();
  this->graph_order.clear();
  this->update_tab_widget();

  // // check app version
  // std::string version_file = json["Hesiod version"];
  // std::string version = "v" + std::to_string(HESIOD_VERSION_MAJOR) + "." +
  //                       std::to_string(HESIOD_VERSION_MINOR) + "." +
  //                       std::to_string(HESIOD_VERSION_PATCH);
  // if (version != version_file)
  //   LOG->warn(
  //       "Hesiod version for this json {} is different for this executable version {}",
  //       version_file,
  //       version);

  // keep deserializing
  this->headless = json["headless"];
  this->id_count = json["id_count"];

  // graph order
  std::vector<std::string> gid_order = json["graph_order"];

  for (auto &id : gid_order)
  {
    LOG->trace("graph id: {}", id);

    auto config = std::make_shared<hesiod::ModelConfig>();
    auto graph = std::make_shared<hesiod::GraphEditor>("", config);

    // in this order, add the graph editor to the graph manager and
    // then deserialize the graph editor because the Receive nodes, if
    // any, need a reference to the broadcast_params of the
    // GraphManager, which is provided to the graph editor when
    // added...
    this->add_graph_editor(graph, id);
    graph->json_from(json["GraphEditors"][id]);
  }
}

nlohmann::json GraphManager::json_to() const
{
  nlohmann::json json;

  json["headless"] = this->headless;
  json["id_count"] = this->id_count;
  json["graph_order"] = this->graph_order;

  // graphs
  nlohmann::json json_graphs;
  for (auto &[id, graph] : this->graphs)
    json_graphs[id] = graph->json_to();

  json["GraphEditors"] = json_graphs;

  // std::cout << json.dump(4) << "\n";

  return json;
}

void GraphManager::load_from_file(const std::string &fname)
{
  // load json
  nlohmann::json json;
  std::ifstream  file(fname);

  if (file.is_open())
  {
    file >> json;
    file.close();
    LOG->trace("JSON successfully loaded from {}", fname);
  }

  this->json_from(json["GraphManager"]);

  // update graphs
  for (auto &[id, graph] : this->graphs)
    graph->update();
}

void GraphManager::on_broadcast_node_updated(const std::string &graph_id,
                                             const std::string &tag)
{
  LOG->trace("GraphManager::on_broadcast_node_updated: broadcasting {}", tag);

  for (auto &[gid, graph] : this->graphs)
    if (true) // gid != graph_id)
    {
      // prevent any broadcast from a top layer to a sublayer, this
      // could lead to endless loop in case of cross-broadcast
      if (this->is_graph_above(gid, graph_id))
      {
        graph->on_broadcast_node_updated(tag);
      }
      else
      {
        LOG->warn("GraphManager::on_broadcast_node_updated: broacast prevented, graph "
                  "[{}] is below graph [{}], check graph order",
                  gid,
                  graph_id);
      }
    }
}

void GraphManager::on_new_broadcast_tag(const std::string     &tag,
                                        const hmap::Terrain   *t_source,
                                        const hmap::Heightmap *h_source)
{
  LOG->trace("GraphManager::on_new_broadcast_tag: tag {}", tag);

  this->broadcast_params[tag] = BroadcastParam(t_source, h_source);
  this->update_receive_nodes_tag_list();
}

void GraphManager::on_remove_broadcast_tag(const std::string &tag)
{
  LOG->trace("GraphManager::on_remove_broadcast_tag: tag {}", tag);

  this->broadcast_params.erase(tag);
  this->update_receive_nodes_tag_list();
}

void GraphManager::remove_graph_editor(const std::string &id)
{
  LOG->trace("Removing graph editor: {}", id);

  if (this->is_graph_id_available(id))
    return;

  this->graph_order.erase(
      std::remove(this->graph_order.begin(), this->graph_order.end(), id),
      this->graph_order.end());

  this->graphs.erase(id);

  this->update_tab_widget();
}

void GraphManager::save_to_file(const std::string &fname) const
{
  // fill-in json with graph data
  nlohmann::json json;
  json["GraphManager"] = this->json_to();

  // add global infos
  json["Hesiod version"] = "v" + std::to_string(HESIOD_VERSION_MAJOR) + "." +
                           std::to_string(HESIOD_VERSION_MINOR) + "." +
                           std::to_string(HESIOD_VERSION_PATCH);

  // save file
  std::ofstream file(fname);

  if (file.is_open())
  {
    file << json.dump(4);
    file.close();
    LOG->trace("JSON successfully written to {}", fname);
  }
  else
    LOG->error("Could not open file {} to load JSON", fname);
}

void GraphManager::set_fname_path(const std::filesystem::path &new_fname_path)
{
  this->fname_path = new_fname_path;

  std::string title = "Hesiod";
  if (!this->fname_path.empty())
    title += " - " + this->fname_path.string();

  // check that the main window exist in this case to avoid an endless loop...
  if (MainWindow::exists())
    MainWindow::instance()->set_title(title.c_str());
}

void GraphManager::set_fname(const std::string &new_fname_path)
{
  this->set_fname_path(std::filesystem::path(new_fname_path));
}

void GraphManager::set_graph_order(const std::vector<std::string> &new_graph_order)
{
  this->graph_order = new_graph_order;
  this->update_tab_widget();
}

void GraphManager::set_selected_tab(const std::string &id)
{
  if (!this->tab_widget)
    return;

  for (int i = 0; i < this->tab_widget->count(); ++i)
  {
    QString tab_label = tab_widget->tabText(i);
    if (tab_label.toStdString() == id)
      this->tab_widget->setCurrentIndex(i);
  }
}

void GraphManager::update_receive_nodes_tag_list()
{
  LOG->trace("GraphManager::update_receive_nodes_tag_list");

  // update the tag list for all the Receive nodes of the graphs
  for (auto &[gid, graph] : this->graphs)
    for (auto &[nid, node] : graph->get_nodes())
      if (node->get_label() == "Receive")
      {
        ReceiveNode *p_rnode = graph->get_node_ref_by_id<ReceiveNode>(nid);

        std::vector<std::string> tags = {};
        for (auto &[tag, _] : this->broadcast_params)
          tags.push_back(tag);

        p_rnode->update_tag_list(tags);
      }
}

void GraphManager::update_tab_widget()
{
  LOG->trace("GraphManager::update_tab_widget");

  if (!this->tab_widget)
    return;

  // TODO optimize this

  // backup currently selected tab to set it back afterwards
  QString current_tab_label = this->tab_widget->tabText(this->tab_widget->currentIndex());

  // remove everything
  while (this->tab_widget->count() > 0)
    this->tab_widget->removeTab(0);

  // repopulate tabs
  for (auto &id : this->graph_order)
  {
    LOG->trace("updating tab for graph: {}", id);

    QWidget     *tab = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(this->graphs.at(id)->get_p_viewer());
    tab->setLayout(layout);
    this->tab_widget->addTab(tab, id.c_str());

    // select this tab if it was selected before
    if (id == current_tab_label.toStdString())
      this->tab_widget->setCurrentWidget(tab);
  }
}

} // namespace hesiod
