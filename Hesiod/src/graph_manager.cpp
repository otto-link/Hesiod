/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>
#include <iostream>

#include <QHBoxLayout>

#include "hesiod/graph_manager.hpp"
#include "hesiod/gui/main_window.hpp"
#include "hesiod/logger.hpp"

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
  std::string graph_id = id;

  // use node pointer as ID if none is provided
  if (id.empty())
    graph_id = std::to_string(this->id_count++);

  // check if the ID is available
  if (!this->is_graph_id_available(graph_id))
    throw std::runtime_error("Graph ID already used: " + graph_id);

  // ddd the graph to the map and store the ID within the graph (in case of)
  this->graphs[graph_id] = p_graph_editor;
  p_graph_editor->set_id(graph_id);

  this->graph_order.push_back(graph_id);

  this->update_tab_widget();

  return graph_id;
}

std::string GraphManager::add_graph_editor_after(
    const std::shared_ptr<GraphEditor> &p_graph_editor,
    const std::string                  &existing_graph_id,
    const std::string                  &id)
{
  // check the reference graph id indeed exists
  if (this->is_graph_id_available(existing_graph_id))
    throw std::runtime_error("Unknown graph ID: " + existing_graph_id);

  std::string graph_id = this->add_graph_editor(p_graph_editor, id);

  // fix graph_order
  this->graph_order.pop_back();
  auto it = std::find(this->graph_order.begin(),
                      this->graph_order.end(),
                      existing_graph_id);
  this->graph_order.insert(it + 1, id);

  this->update_tab_widget();

  return graph_id;
}

std::string GraphManager::add_graph_editor_before(
    const std::shared_ptr<GraphEditor> &p_graph_editor,
    const std::string                  &existing_graph_id,
    const std::string                  &id)
{
  // check the reference graph id indeed exists
  if (this->is_graph_id_available(existing_graph_id))
    throw std::runtime_error("Unknown graph ID: " + existing_graph_id);

  std::string graph_id = this->add_graph_editor(p_graph_editor, id);

  // fix graph_order
  this->graph_order.pop_back();
  auto it = std::find(this->graph_order.begin(),
                      this->graph_order.end(),
                      existing_graph_id);
  this->graph_order.insert(it, id);

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
  std::vector<std::string> gid_order = json["graph_order"];

  for (auto &id : gid_order)
  {
    LOG->trace("graph id: {}", id);

    auto config = std::make_shared<hesiod::ModelConfig>();
    auto graph = std::make_shared<hesiod::GraphEditor>("", config);

    graph->json_from(json["GraphEditors"][id]);
    this->add_graph_editor(graph, id);
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

  std::cout << json.dump(4) << "\n";

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
