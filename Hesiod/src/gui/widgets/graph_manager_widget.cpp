/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QMenu>
#include <QPushButton>

#include "hesiod/gui/widgets/coord_frame_widget.hpp"
#include "hesiod/gui/widgets/graph_manager_widget.hpp"
#include "hesiod/gui/widgets/model_config_widget.hpp"
#include "hesiod/gui/widgets/string_input_dialog.hpp"
#include "hesiod/logger.hpp"

#define MINIMUM_WIDTH 256

namespace hesiod
{

GraphManagerWidget::GraphManagerWidget(GraphManager *p_graph_manager, QWidget *parent)
    : QWidget(parent), p_graph_manager(p_graph_manager)
{
  if (!p_graph_manager)
    throw std::runtime_error("p_graph_manager is nullptr");

  // --- build widget layout
  int row = 0;

  QGridLayout *layout = new QGridLayout(this);

  // left pan
  this->coord_frame_widget = new CoordFrameWidget(p_graph_manager, this);
  layout->addWidget(this->coord_frame_widget, row, 0, 2, 1);

  // right pan
  this->list_widget = new QListWidget(this);
  this->list_widget->setStyleSheet("QListWidget::item { border: 1px solid #5B5B5B; "
                                   "padding-bottom: 20px; padding-top: 20px;}");
  this->list_widget->setViewMode(QListView::ListMode);
  this->list_widget->setDragDropMode(QAbstractItemView::InternalMove);
  this->list_widget->setContextMenuPolicy(Qt::CustomContextMenu);
  this->list_widget->setMinimumSize(MINIMUM_WIDTH, this->list_widget->height());

  // populate with graph editor names
  for (auto &id : this->p_graph_manager->get_graph_order())
    this->add_list_item(id);

  layout->addWidget(this->list_widget, row++, 1, 1, 3);

  // buttons
  QPushButton *new_button = new QPushButton("New");
  layout->addWidget(new_button, row, 1);

  QPushButton *zoom_button = new QPushButton("Zoom");
  layout->addWidget(zoom_button, row, 2);

  this->apply_button = new QPushButton("Apply");
  // this->apply_button->setStyleSheet("QPushButton:enabled {background-color:
  // #4772b3;}");
  this->apply_button->setStyleSheet("QPushButton:enabled {background-color: red;}");
  layout->addWidget(apply_button, row, 3);

  row++;

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

  this->connect(apply_button,
                &QPushButton::pressed,
                this,
                &GraphManagerWidget::on_apply_changes);

  this->connect(new_button,
                &QPushButton::pressed,
                this,
                &GraphManagerWidget::on_new_graph_request);

  this->connect(p_graph_manager,
                &GraphManager::has_been_cleared,
                this,
                &GraphManagerWidget::reset);

  this->connect(p_graph_manager,
                &GraphManager::has_been_loaded_from_file,
                this,
                &GraphManagerWidget::reset);

  // frames canvas
  this->connect(this->coord_frame_widget,
                &CoordFrameWidget::has_changed,
                this,
                [this]() { this->set_is_dirty(true); });

  this->connect(zoom_button,
                &QPushButton::pressed,
                this->coord_frame_widget,
                &CoordFrameWidget::on_zoom_to_content);

  // clean state
  this->set_is_dirty(false);
}

void GraphManagerWidget::add_list_item(const std::string &id)
{
  auto *item = new QListWidgetItem();
  item->setText(id.c_str());
  this->list_widget->addItem(item);
}

void GraphManagerWidget::on_apply_changes()
{
  LOG->trace("GraphManagerWidget::on_apply_changes");

  // retrieve coordinate frame parameters from the frames canvas
  for (auto &[id, graph] : this->p_graph_manager->get_graphs())
  {
    LOG->trace("id: {}", id);

    QPointF origin_out;
    QPointF size_out;
    float   angle_out;

    this->coord_frame_widget->get_frame_ref(id)->get_frame_parameter(origin_out,
                                                                     size_out,
                                                                     angle_out);

    graph->set_origin(hmap::Vec2<float>(origin_out.x(), origin_out.y()));
    graph->set_size(hmap::Vec2<float>(size_out.x(), size_out.y()));
    graph->set_rotation_angle(angle_out);
  }

  // update the graph from bottom to top
  for (auto &id : this->p_graph_manager->get_graph_order())
    this->p_graph_manager->get_graph_ref_by_id(id)->update();

  // clean state
  this->set_is_dirty(false);
}

void GraphManagerWidget::on_item_double_clicked(QListWidgetItem *item)
{
  this->p_graph_manager->set_selected_tab(item->text().toStdString());
}

void GraphManagerWidget::on_list_reordered(const QModelIndex &,
                                           int,
                                           int,
                                           const QModelIndex &,
                                           int)
{
  LOG->trace("GraphManagerWidget::on_list_reordered");

  // retrieve new graph order
  std::vector<std::string> new_graph_order = {};

  for (int i = 0; i < this->list_widget->count(); ++i)
    new_graph_order.push_back(this->list_widget->item(i)->text().toStdString());

  // update storage
  this->p_graph_manager->set_graph_order(new_graph_order);

  // update frames canvas
  this->coord_frame_widget->update_frames_z_depth();
  this->set_is_dirty(true);
}

void GraphManagerWidget::on_new_graph_request()
{
  // get ID from user
  std::vector<std::string> invalid_graph_ids = this->p_graph_manager->get_graph_order();

  StringInputDialog id_dialog("Enter new graph ID", invalid_graph_ids);
  std::string       new_graph_id;

  {
    int ret = id_dialog.exec();
    if (!ret)
      return;
    else
      new_graph_id = id_dialog.get_text_std_string();
  }

  // get config from user
  auto              config = std::make_shared<hesiod::ModelConfig>();
  ModelConfigWidget config_editor(config.get());

  {
    int ret = config_editor.exec();
    if (!ret)
      return;
  }

  // create new graph
  auto graph = std::make_shared<hesiod::GraphEditor>(new_graph_id, config);
  this->p_graph_manager->add_graph_editor(graph, new_graph_id);

  // add to list widget
  this->add_list_item(new_graph_id);

  // add to frames canvas
  this->coord_frame_widget->add_frame(new_graph_id);
}

void GraphManagerWidget::reset()
{
  this->list_widget->clear();

  for (auto &id : this->p_graph_manager->get_graph_order())
    this->add_list_item(id);

  this->coord_frame_widget->reset();
}

void GraphManagerWidget::set_is_dirty(bool new_is_dirty)
{
  this->is_dirty = new_is_dirty;
  this->apply_button->setEnabled(this->is_dirty);
  this->update();
}

void GraphManagerWidget::show_context_menu(const QPoint &pos)
{
  LOG->trace("GraphManagerWidget::show_context_menu");

  QListWidgetItem *item = this->list_widget->itemAt(pos);

  if (!item)
    return;

  std::string selected_id = item ? item->text().toStdString() : "";
  LOG->trace("selected_id: {}", selected_id);

  QMenu    menu;
  QAction *new_action = menu.addAction("New");
  QAction *set_focus_action = menu.addAction("Show in graph editor");
  QAction *delete_action = menu.addAction("Delete");

  QPoint   global_pos = this->list_widget->mapToGlobal(pos);
  QAction *selected_action = menu.exec(global_pos);

  if (selected_action == delete_action)
  {
    delete this->list_widget->takeItem(this->list_widget->row(item));
    this->p_graph_manager->remove_graph_editor(selected_id);
    this->coord_frame_widget->remove_frame(selected_id);
  }
  else if (selected_action == set_focus_action)
  {
    this->p_graph_manager->set_selected_tab(item->text().toStdString());
  }
  else if (selected_action == new_action)
  {
    this->on_new_graph_request();
  }
}

} // namespace hesiod
