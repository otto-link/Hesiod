/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QHBoxLayout>

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/gui/widgets/graph_editor_widget.hpp"
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/node_settings_widget.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph/graph_node.hpp"

namespace hesiod
{

GraphEditorWidget::GraphEditorWidget(std::weak_ptr<GraphNode> p_graph_node,
                                     QWidget                 *parent)
    : QWidget(parent), p_graph_node(p_graph_node)
{
  Logger::log()->trace("GraphEditorWidget::GraphEditorWidget");

  auto gno = this->p_graph_node.lock();
  if (!gno)
    return;

  this->setup_layout();
  this->setup_connections();
}

GraphNodeWidget *GraphEditorWidget::get_graph_node_widget() const
{
  return this->graph_node_widget;
}

NodeSettingsWidget *GraphEditorWidget::get_node_settings_widget() const
{
  return this->node_settings_widget;
}

void GraphEditorWidget::json_from(nlohmann::json const &json)
{
  // GraphNodeWidget
  if (this->graph_node_widget)
  {
    const std::string graph_id = this->graph_node_widget->get_id();
    if (json.contains(graph_id))
      this->graph_node_widget->json_from(json[graph_id]);
  }
}

nlohmann::json GraphEditorWidget::json_to() const
{
  nlohmann::json json;

  // GraphNodeWidget
  if (this->graph_node_widget)
    json[this->graph_node_widget->get_id()] = this->graph_node_widget->json_to();

  return json;
}

void GraphEditorWidget::setup_connections()
{
  Logger::log()->trace("GraphEditorWidget::setup_connections");

  if (!this->graph_node_widget)
    return;

  // nothing here... for now
}

void GraphEditorWidget::setup_layout()
{
  Logger::log()->trace("GraphEditorWidget::setup_layout");

  auto gno = this->p_graph_node.lock();
  if (!gno)
    return;

  QHBoxLayout *layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  this->setLayout(layout);

  this->graph_node_widget = new GraphNodeWidget(gno->get_shared());
  layout->addWidget(this->graph_node_widget);

  this->node_settings_widget = new NodeSettingsWidget(this->graph_node_widget);

  this->node_settings_widget->setObjectName("NodeSettingsRoot");
  std::string style = std::format(
      "#NodeSettingsRoot > QWidget {{ border-left: 1px solid {}; }}",
      HSD_CTX.app_settings.colors.border.name().toStdString());
  this->node_settings_widget->setStyleSheet(style.c_str());

  layout->addWidget(this->node_settings_widget);

  this->node_settings_widget->setVisible(
      HSD_CTX.app_settings.node_editor.show_node_settings_pan);
}

} // namespace hesiod
