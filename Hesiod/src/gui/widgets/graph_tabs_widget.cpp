/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QHBoxLayout>

#include "gnodegui/style.hpp"

#include "hesiod/gui/style.hpp"
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/graph_tabs_widget.hpp"
#include "hesiod/gui/widgets/node_settings_widget.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_manager.hpp"
#include "hesiod/model/graph_node.hpp"
#include "hesiod/model/nodes/receive_node.hpp"

namespace hesiod
{

GraphTabsWidget::GraphTabsWidget(GraphManager *p_graph_manager, QWidget *parent)
    : QWidget(parent), p_graph_manager(p_graph_manager)
{
  Logger::log()->trace("GraphTabsWidget::GraphTabsWidget");

  // styles (GNodeGUI)
  GN_STYLE->viewer.add_new_icon = false;
  GN_STYLE->viewer.add_load_save_icons = false;
  GN_STYLE->viewer.add_group = false;
  GN_STYLE->node.color_port_data = data_color_map;
  GN_STYLE->node.color_category = category_color_map;

  QHBoxLayout *layout = new QHBoxLayout(this);
  this->setLayout(layout);

  this->tab_widget = new QTabWidget(this);
  layout->addWidget(this->tab_widget);

  this->update_tab_widget();
}

void GraphTabsWidget::clear()
{
  Logger::log()->trace("GraphTabsWidget::clear");

  // clear tabs
  while (this->tab_widget->count() > 0)
    this->tab_widget->removeTab(0);

  for (auto &[id, gnw] : this->graph_node_widget_map)
  {
    if (gnw)
    {
      gnw->clear_graphic_scene();
      gnw->close();
    }
  }

  this->graph_node_widget_map.clear();

  // also clear settings widgets map (close widgets first)
  for (auto &[id, sw] : this->node_settings_widget_map)
  {
    if (sw)
      sw->close();
  }
  this->node_settings_widget_map.clear();
}

std::string GraphTabsWidget::get_selected_graph_id() const
{
  QString selected_tab_text = this->tab_widget->tabText(this->tab_widget->currentIndex());
  return selected_tab_text.toStdString();
}

void GraphTabsWidget::on_textures_request(const std::vector<std::string> &texture_paths)
{
  Logger::log()->trace("GraphTabsWidget::on_textures_request");

  if (texture_paths.empty())
    return;

  std::string graph_id = this->get_selected_graph_id();

  GraphNodeWidget *gnw = this->graph_node_widget_map.at(graph_id);
  if (gnw)
    gnw->add_import_texture_nodes(texture_paths);
  else
    Logger::log()->error("GraphTabsWidget::on_textures_request: dangling ptr");
}

void GraphTabsWidget::json_from(nlohmann::json const &json)
{
  this->update_tab_widget();

  if (json.contains("graph_node_widgets"))
  {
    for (auto &[id, gnw] : this->graph_node_widget_map)
    {
      if (json["graph_node_widgets"].contains(id))
      {
        if (gnw)
          gnw->json_from(json["graph_node_widgets"][id]);
      }
      else
      {
        Logger::log()->error("Missing graph node widgets json element by key \"{}\"", id);
      }
    }
  }
  else
  {
    Logger::log()->error("Missing graph_node_widgets in json");
  }
}

nlohmann::json GraphTabsWidget::json_to() const
{
  nlohmann::json json;

  for (auto &[id, gnw] : this->graph_node_widget_map)
    if (gnw)
      json["graph_node_widgets"][id] = gnw->json_to();

  return json;
}

void GraphTabsWidget::on_copy_buffer_has_changed(const nlohmann::json &new_json)
{
  Logger::log()->trace("GraphTabsWidget::on_copy_buffer_has_changed");

  // redispatch the copy buffer to all the graphs
  for (auto &[_, gnw] : this->graph_node_widget_map)
    if (gnw)
      gnw->set_json_copy_buffer(new_json);
}

void GraphTabsWidget::on_has_been_cleared(const std::string &graph_id)
{
  Q_EMIT this->has_been_cleared(graph_id);
  Q_EMIT this->has_changed();
}

void GraphTabsWidget::on_new_node_created(const std::string &graph_id,
                                          const std::string &id)
{
  Logger::log()->trace("GraphTabsWidget::on_new_node_created");

  // check if it's a Receive node to update its tag list
  auto it_graph = this->p_graph_manager->get_graph_nodes().find(graph_id);
  if (it_graph == this->p_graph_manager->get_graph_nodes().end())
  {
    Logger::log()->critical("GraphTabsWidget::on_new_node_created: graph {} not found",
                            graph_id);
    return;
  }

  BaseNode *p_node = it_graph->second->get_node_ref_by_id<BaseNode>(id);
  if (p_node)
  {
    if (p_node->get_node_type() == "Receive")
      this->update_receive_nodes_tag_list();

    Q_EMIT this->new_node_created(graph_id, id);
    Q_EMIT this->has_changed();
  }
  else
  {
    Logger::log()->critical(
        "GraphTabsWidget::on_new_node_created: the node just created is nullptr");
  }
}

void GraphTabsWidget::on_node_deleted(const std::string &graph_id, const std::string &id)
{
  Q_EMIT this->node_deleted(graph_id, id);
  Q_EMIT this->has_changed();
}

void GraphTabsWidget::set_show_node_settings_widget(bool new_state)
{
  this->show_node_settings_widget = new_state;
  this->update_tab_widget();
}

void GraphTabsWidget::set_selected_tab(const std::string &graph_id)
{
  for (int i = 0; i < this->tab_widget->count(); ++i)
  {
    QString tab_label = tab_widget->tabText(i);
    if (tab_label.toStdString() == graph_id)
      this->tab_widget->setCurrentIndex(i);
  }
}

void GraphTabsWidget::show_viewport()
{
  Logger::log()->trace("GraphTabsWidget::show_viewport");

  if (!this->graph_node_widget_map.empty() && this->graph_node_widget_map.begin()->second)
    this->graph_node_widget_map.begin()->second->on_viewport_request();
}

QSize GraphTabsWidget::sizeHint() const { return QSize(1024, 1024); }

void GraphTabsWidget::update_receive_nodes_tag_list()
{
  Logger::log()->trace("GraphTabsWidget::update_receive_nodes_tag_list");

  // update the tag list for all the Receive nodes of the graphs
  for (auto &[gid, graph] : this->p_graph_manager->get_graph_nodes())
    for (auto &[nid, node] : graph->get_nodes())
      if (node->get_label() == "Receive")
      {
        ReceiveNode *p_rnode = graph->get_node_ref_by_id<ReceiveNode>(nid);

        std::vector<std::string> tags = {};
        for (auto &[tag, _] : this->p_graph_manager->get_broadcast_params())
          tags.push_back(tag);

        if (p_rnode)
          p_rnode->update_tag_list(tags);
      }
}

void GraphTabsWidget::update_tab_widget()
{
  Logger::log()->trace("GraphTabsWidget::update_tab_widget");

  if (!this->tab_widget)
    return;

  // backup currently selected tab to set it back afterwards
  QString current_tab_label;
  if (this->tab_widget->count() > 0 && this->tab_widget->currentIndex() >= 0)
    current_tab_label = this->tab_widget->tabText(this->tab_widget->currentIndex());

  // remove everything
  while (this->tab_widget->count() > 0)
    this->tab_widget->removeTab(0);

  // clean-up stored GraphNodeWidget
  std::vector<std::string> id_to_erase = {};

  for (auto &[id, _] : this->graph_node_widget_map)
  {
    // ditch if the corresponding GraphNode no longer exists
    GraphNode *p_graph_node = this->p_graph_manager->get_graph_ref_by_id(id);
    if (!p_graph_node)
      id_to_erase.push_back(id);
  }

  for (auto &id : id_to_erase)
  {
    if (auto it = this->graph_node_widget_map.find(id);
        it != this->graph_node_widget_map.end())
    {
      if (it->second)
        it->second->close();
      this->graph_node_widget_map.erase(it);
    }

    if (auto it2 = this->node_settings_widget_map.find(id);
        it2 != this->node_settings_widget_map.end())
    {
      if (it2->second)
        it2->second->close();
      this->node_settings_widget_map.erase(it2);
    }
  }

  // repopulate tabs
  for (auto &id : this->p_graph_manager->get_graph_order())
  {
    Logger::log()->trace("updating tab for graph: {}", id);

    // generate a graph editor if not already available
    if (!this->graph_node_widget_map.contains(id))
    {
      GraphNode *p_graph_node = this->p_graph_manager->get_graph_ref_by_id(id);
      if (!p_graph_node)
      {
        Logger::log()->error(
            "GraphTabsWidget::update_tab_widget: graph '{}' not found, skipping",
            id);
        continue;
      }

      // set 'this' as parent so the widget is owned by the widget tree.
      this->graph_node_widget_map[id] = new GraphNodeWidget(p_graph_node, this);

      // connections / model
      this->connect(p_graph_node,
                    &GraphNode::new_broadcast_tag,
                    [this]() { this->update_receive_nodes_tag_list(); });

      this->connect(p_graph_node,
                    &GraphNode::remove_broadcast_tag,
                    [this]() { this->update_receive_nodes_tag_list(); });

      // connections / GUI
      this->connect(this->graph_node_widget_map.at(id),
                    &GraphNodeWidget::has_been_cleared,
                    this,
                    &GraphTabsWidget::on_has_been_cleared);

      this->connect(this->graph_node_widget_map.at(id),
                    &GraphNodeWidget::new_node_created,
                    this,
                    &GraphTabsWidget::on_new_node_created);

      this->connect(this->graph_node_widget_map.at(id),
                    &GraphNodeWidget::node_deleted,
                    this,
                    &GraphTabsWidget::on_node_deleted);

      this->connect(this->graph_node_widget_map.at(id),
                    &GraphNodeWidget::copy_buffer_has_changed,
                    this,
                    &GraphTabsWidget::on_copy_buffer_has_changed);
    }

    // build layout
    QWidget *tab = new QWidget();
    auto    *layout = new QHBoxLayout(tab);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto *graph_widget = this->graph_node_widget_map.at(id);
    if (!graph_widget)
    {
      Logger::log()->error(
          "GraphTabsWidget::update_tab_widget: graph widget for '{}' is nullptr",
          id);
      continue;
    }

    // Give main area most of the space
    layout->addWidget(graph_widget, 3);

    if (this->show_node_settings_widget)
    {
      // either reuse or create the settings widget. parent it to the tab so that when
      // the tab is destroyed the widget is as well (but keep pointer in the map).
      NodeSettingsWidget *settings_widget = nullptr;

      auto it = this->node_settings_widget_map.find(id);
      if (it == this->node_settings_widget_map.end() || it->second == nullptr)
      {
        settings_widget = new NodeSettingsWidget(graph_widget, tab);
        this->node_settings_widget_map[id] = settings_widget;
      }
      else
      {
        settings_widget = it->second;
        // ensure parent is the current tab (in case it was created with different parent)
        if (settings_widget->parentWidget() != tab)
          settings_widget->setParent(tab);
      }

      // Make settings compact vertically and constrain width
      settings_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
      settings_widget->setMinimumWidth(300);

      // Add without stretch so it keeps its preferred width
      layout->addWidget(settings_widget, 0);
      settings_widget->update_content();
    }

    tab->setLayout(layout);
    this->tab_widget->addTab(tab, QString::fromStdString(id));

    // restore selection if this tab was selected previously
    if (!current_tab_label.isEmpty() && current_tab_label.toStdString() == id)
      this->tab_widget->setCurrentWidget(tab);
  }
}

void GraphTabsWidget::zoom_to_content()
{
  for (auto &[id, gnw] : this->graph_node_widget_map)
  {
    if (gnw)
      gnw->zoom_to_content();
  }
}

} // namespace hesiod
