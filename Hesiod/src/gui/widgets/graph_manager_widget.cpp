/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QMenu>

#include "hesiod/gui/widgets/graph_manager_widget.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

GraphManagerWidget::GraphManagerWidget(GraphManager *p_graph_manager, QWidget *parent)
    : QWidget(parent), p_graph_manager(p_graph_manager)
{
  if (!p_graph_manager)
    throw std::runtime_error("p_graph_manager is nullptr");

  // --- build widget layout
  QGridLayout *layout = new QGridLayout(this);

  this->list_widget = new QListWidget(this);
  this->list_widget->setStyleSheet(
      "QListWidget::item { padding-bottom: 20px; padding-top: 20px; }");
  this->list_widget->setViewMode(QListView::ListMode);
  this->list_widget->setDragDropMode(QAbstractItemView::InternalMove);
  this->list_widget->setContextMenuPolicy(Qt::CustomContextMenu);

  layout->addWidget(this->list_widget);

  // populate with graph editor names
  for (auto &id : this->p_graph_manager->get_graph_order())
    this->add_list_item(id);

  this->setLayout(layout);

  // --- connections

  this->connect(this->list_widget,
                &QListWidget::customContextMenuRequested,
                this,
                &GraphManagerWidget::show_context_menu);

  this->connect(this->list_widget->model(),
                &QAbstractItemModel::rowsMoved,
                this,
                &GraphManagerWidget::on_list_reordered);

  this->connect(this->list_widget,
                &QListWidget::itemDoubleClicked,
                this,
                &GraphManagerWidget::on_item_double_clicked);
}

void GraphManagerWidget::add_list_item(const std::string &id)
{
  auto *item = new QListWidgetItem();
  item->setText(id.c_str());
  this->list_widget->addItem(item);
}

void GraphManagerWidget::on_item_double_clicked(QListWidgetItem *item)
{
  qDebug() << "Double-clicked on:" << item->text();
  this->p_graph_manager->set_selected_tab(item->text().toStdString());
}

void GraphManagerWidget::on_list_reordered(const QModelIndex &,
                                           int,
                                           int,
                                           const QModelIndex &,
                                           int)
{
  LOG->trace("GraphManagerWidget::on_list_reordered");

  std::vector<std::string> new_graph_order = {};

  for (int i = 0; i < this->list_widget->count(); ++i)
    new_graph_order.push_back(this->list_widget->item(i)->text().toStdString());

  this->p_graph_manager->set_graph_order(new_graph_order);
}

void GraphManagerWidget::show_context_menu(const QPoint &pos)
{
  LOG->trace("GraphManagerWidget::show_context_menu");

  QListWidgetItem *item = this->list_widget->itemAt(pos);
  if (!item)
    return;

  std::string selected_id = item->text().toStdString();
  LOG->trace("selected_id: {}", selected_id);

  QMenu    menu;
  QAction *new_action = menu.addAction("New");
  QAction *delete_action = menu.addAction("Delete");
  QAction *selected_action = menu.exec(mapToGlobal(pos));

  if (selected_action == delete_action)
  {
    delete this->list_widget->takeItem(this->list_widget->row(item));
    this->p_graph_manager->remove_graph_editor(selected_id);
  }
  else if (selected_action == new_action)
  {
    // TODO - add QDialog to select ID and config

    // add graph editor
    auto        config = std::make_shared<hesiod::ModelConfig>();
    auto        graph = std::make_shared<hesiod::GraphEditor>("", config);
    std::string new_id = this->p_graph_manager->add_graph_editor(graph, "");

    // add to list widget
    this->add_list_item(new_id);
  }
}

} // namespace hesiod
