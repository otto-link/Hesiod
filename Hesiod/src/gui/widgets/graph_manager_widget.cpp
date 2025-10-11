/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QMenu>
#include <QPushButton>
#include <QSettings>

#include "hesiod/gui/main_window.hpp"
#include "hesiod/gui/widgets/coord_frame_widget.hpp"
#include "hesiod/gui/widgets/export_param_widget.hpp"
#include "hesiod/gui/widgets/graph_manager_widget.hpp"
#include "hesiod/gui/widgets/model_config_widget.hpp"
#include "hesiod/gui/widgets/string_input_dialog.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/export_param.hpp"
#include "hesiod/model/graph_manager.hpp"
#include "hesiod/model/graph_node.hpp"

#define MINIMUM_WIDTH 384

namespace hesiod
{

GraphManagerWidget::GraphManagerWidget(GraphManager *p_graph_manager, QWidget *parent)
    : QWidget(parent), p_graph_manager(p_graph_manager)
{
  if (!p_graph_manager)
    throw std::runtime_error("p_graph_manager is nullptr");

  this->setWindowTitle(tr("Hesiod - GraphManager"));

  // --- build widget layout
  int row = 0;

  QGridLayout *layout = new QGridLayout(this);

  // left pan
  this->coord_frame_widget = new CoordFrameWidget(p_graph_manager, this);
  layout->addWidget(this->coord_frame_widget, row, 0, 2, 1);

  // right pan
  this->list_widget = new QListWidget(this);
  this->list_widget->setStyleSheet(
      "QListWidget::item { border: 1px solid #5B5B5B; color: transparent; }");
  this->list_widget->setViewMode(QListView::ListMode);
  this->list_widget->setDragDropMode(QAbstractItemView::InternalMove);
  this->list_widget->setContextMenuPolicy(Qt::CustomContextMenu);
  this->list_widget->setMinimumSize(MINIMUM_WIDTH, this->list_widget->height());

  // populate with graph editor names
  for (auto &id : this->p_graph_manager->get_graph_order())
    this->add_list_item(id);

  layout->addWidget(this->list_widget, row++, 1, 1, 4);

  // buttons
  QPushButton *new_button = new QPushButton("New");
  layout->addWidget(new_button, row, 1);

  QPushButton *zoom_button = new QPushButton("Zoom");
  layout->addWidget(zoom_button, row, 2);

  QPushButton *export_button = new QPushButton("Flatten/export");
  layout->addWidget(export_button, row, 3);

  this->apply_button = new QPushButton("Apply");
  this->apply_button->setStyleSheet("QPushButton:enabled {background-color: red;}");
  layout->addWidget(apply_button, row, 4);

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

  this->connect(export_button,
                &QPushButton::pressed,
                this,
                &GraphManagerWidget::on_export);

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

  // restore window geometry
  this->restore_window_state();
}

void GraphManagerWidget::add_list_item(const std::string &id)
{
  // create the list item
  auto *item = new QListWidgetItem();
  item->setText(id.c_str());

  // create the custom widget to embed inside the item
  GraphQListWidget *widget = new GraphQListWidget(
      this->p_graph_manager->get_graph_nodes().at(id).get());

  item->setSizeHint(widget->sizeHint());

  // add the item and set the widget
  this->list_widget->addItem(item);
  this->list_widget->setItemWidget(item, widget);

  // connections
  this->connect(
      widget,
      &GraphQListWidget::bg_image_updated,
      this,
      [this](const std::string &id, QImage image)
      { this->coord_frame_widget->get_frame_ref(id)->set_background_image(image); });

  this->connect(widget,
                &GraphQListWidget::has_changed,
                this,
                [this]() { Q_EMIT this->has_changed(); });

  this->connect(this->p_graph_manager->get_graph_nodes().at(id).get(),
                &GraphNode::update_finished, // TODO NOPE
                widget,
                [widget](const std::string & /* graph_id */)
                { widget->on_combobox_changed(); });
}

void GraphManagerWidget::clear()
{
  Logger::log()->trace("GraphManagerWidget::clear");

  this->coord_frame_widget->clear();
  this->list_widget->clear();
}

void GraphManagerWidget::closeEvent(QCloseEvent *event)
{
  this->save_window_state();
  QWidget::closeEvent(event);
  Q_EMIT this->window_closed();
}

void GraphManagerWidget::hideEvent(QHideEvent *event)
{
  this->save_window_state();
  QWidget::hideEvent(event);
}

void GraphManagerWidget::json_from(nlohmann::json const &json)
{
  this->reset();

  for (int i = 0; i < this->list_widget->count(); ++i)
  {
    QListWidgetItem *item = this->list_widget->item(i);
    std::string      id = item->text().toStdString();
    QWidget         *widget = this->list_widget->itemWidget(item);

    if (auto gw_widget = dynamic_cast<GraphQListWidget *>(widget))
      gw_widget->json_from(json["frames"][id]);
  }
}

nlohmann::json GraphManagerWidget::json_to() const
{
  nlohmann::json json;

  for (int i = 0; i < this->list_widget->count(); ++i)
  {
    QListWidgetItem *item = this->list_widget->item(i);
    std::string      id = item->text().toStdString();
    QWidget         *widget = this->list_widget->itemWidget(item);

    if (auto gw_widget = dynamic_cast<GraphQListWidget *>(widget))
      json["frames"][id] = gw_widget->json_to();
  }

  return json;
}

void GraphManagerWidget::on_apply_changes()
{
  Logger::log()->trace("GraphManagerWidget::on_apply_changes");

  // retrieve coordinate frame parameters from the frames canvas
  for (auto &[id, graph] : this->p_graph_manager->get_graph_nodes())
  {
    Logger::log()->trace("id: {}", id);

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

  Q_EMIT this->has_changed();
}

void GraphManagerWidget::on_export()
{
  // TODO move to GraphManager
  Logger::log()->trace("GraphManagerWidget::on_export");

  // define the export parameters using the GUI
  ExportParam export_param = this->p_graph_manager->get_export_param();

  if (export_param.export_path.empty())
    export_param.export_path = "export.png";

  ExportParamWidget param_editor(&export_param);
  int               ret = param_editor.exec();

  if (!ret)
    return;

  // retrieve export ids from the current tags in the GUI
  std::vector<std::tuple<std::string, std::string, std::string>> export_ids = {};

  for (int i = 0; i < this->list_widget->count(); ++i)
  {
    QListWidgetItem *item = this->list_widget->item(i);
    QWidget         *widget = this->list_widget->itemWidget(item);
    if (auto gw_widget = dynamic_cast<GraphQListWidget *>(widget))
    {
      std::string tag = gw_widget->get_current_bg_tag();

      // retrieve node ID from tag
      std::string node_type, node_id, port_id;
      bool        ret = get_ids_from_broadcast_tag(tag, node_type, node_id, port_id);

      if (ret)
      {
        Logger::log()->trace("adding layer: {}", tag);
        export_ids.push_back({gw_widget->get_p_graph_node()->get_id(), node_id, port_id});
      }
      else
      {
        Logger::log()->trace("skipping layer: {}", tag);
      }
    }
  }

  export_param.ids = export_ids;

  this->p_graph_manager->set_export_param(export_param);
  this->p_graph_manager->export_flatten();

  notify("Export/flatten",
         std::format("Flattening and export terminated, saved to file: {}",
                     export_param.export_path.filename().string()));
}

void GraphManagerWidget::on_item_double_clicked(QListWidgetItem *item)
{
  Q_EMIT this->selected_graph_changed(item->text().toStdString());
}

void GraphManagerWidget::on_list_reordered(const QModelIndex &,
                                           int,
                                           int,
                                           const QModelIndex &,
                                           int)
{
  Logger::log()->trace("GraphManagerWidget::on_list_reordered");

  // retrieve new graph order
  std::vector<std::string> new_graph_order = {};

  for (int i = 0; i < this->list_widget->count(); ++i)
    new_graph_order.push_back(this->list_widget->item(i)->text().toStdString());

  // update storage
  this->p_graph_manager->set_graph_order(new_graph_order);

  // update frames canvas
  this->coord_frame_widget->update_frames_z_depth();
  this->set_is_dirty(true);

  Q_EMIT this->list_reordered();
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
  auto graph = std::make_shared<hesiod::GraphNode>(new_graph_id, config);
  this->p_graph_manager->add_graph_node(graph, new_graph_id);

  // add to list widget
  this->add_list_item(new_graph_id);

  // add to frames canvas
  this->coord_frame_widget->add_frame(new_graph_id);

  Q_EMIT this->new_graph_added();
}

void GraphManagerWidget::on_reseed()
{
  if (!this->p_graph_manager)
    return;

  this->p_graph_manager->reseed();
}

void GraphManagerWidget::reset()
{
  this->list_widget->clear();

  for (auto &id : this->p_graph_manager->get_graph_order())
    this->add_list_item(id);

  this->coord_frame_widget->reset();
}

void GraphManagerWidget::restore_window_state()
{
  QSettings settings(HSD_SETTINGS_ORG, HSD_SETTINGS_APP);
  this->restoreGeometry(settings.value("GraphManagerWidget/geometry").toByteArray());
}

void GraphManagerWidget::save_window_state() const
{
  QSettings settings(HSD_SETTINGS_ORG, HSD_SETTINGS_APP);
  settings.setValue("GraphManagerWidget/geometry", this->saveGeometry());
}

void GraphManagerWidget::set_is_dirty(bool new_is_dirty)
{
  this->is_dirty = new_is_dirty;
  this->apply_button->setEnabled(this->is_dirty);
  this->update();
}

void GraphManagerWidget::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);
  this->restore_window_state();
}

void GraphManagerWidget::show_context_menu(const QPoint &pos)
{
  Logger::log()->trace("GraphManagerWidget::show_context_menu");

  QListWidgetItem *item = this->list_widget->itemAt(pos);

  if (!item)
    return;

  std::string selected_id = item ? item->text().toStdString() : "";
  Logger::log()->trace("selected_id: {}", selected_id);

  QMenu    menu;
  QAction *new_action = menu.addAction("New");
  QAction *set_focus_action = menu.addAction("Show in graph editor");
  QAction *delete_action = menu.addAction("Delete");

  QPoint   global_pos = this->list_widget->mapToGlobal(pos);
  QAction *selected_action = menu.exec(global_pos);

  if (selected_action == delete_action)
  {
    delete this->list_widget->takeItem(this->list_widget->row(item));
    this->p_graph_manager->remove_graph_node(selected_id);
    this->coord_frame_widget->remove_frame(selected_id);

    Q_EMIT this->graph_removed();
  }
  else if (selected_action == set_focus_action)
  {
    Q_EMIT this->selected_graph_changed(item->text().toStdString());
  }
  else if (selected_action == new_action)
  {
    this->on_new_graph_request();
  }
}

void GraphManagerWidget::update_combobox(const std::string &graph_id)
{
  Logger::log()->trace("GraphManagerWidget::update_combobox: {}", graph_id);

  // find corresponding item and update its combobox
  for (int i = 0; i < this->list_widget->count(); ++i)
  {
    std::string item_id = this->list_widget->item(i)->text().toStdString();
    if (item_id == graph_id)
    {
      QListWidgetItem *item = this->list_widget->item(i);
      QWidget         *widget = this->list_widget->itemWidget(item);
      if (auto gw_widget = dynamic_cast<GraphQListWidget *>(widget))
        gw_widget->update_combobox();
    }
  }
}

} // namespace hesiod
