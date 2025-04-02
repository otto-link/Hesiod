/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file viewer3d.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include <QListWidget>
#include <QWidget>

#include "hesiod/graph_manager.hpp"
#include "hesiod/gui/widgets/coord_frame_widget.hpp"

#define LIST_ITEM_HEIGHT 64

namespace hesiod
{

class GraphManager; // forward

class GraphManagerWidget : public QWidget
{
  Q_OBJECT

public:
  GraphManagerWidget(GraphManager *p_graph_manager, QWidget *parent = nullptr);

private Q_SLOTS:
  void on_apply_changes();

  void on_item_double_clicked(QListWidgetItem *item);

  void on_list_reordered(const QModelIndex &, int, int, const QModelIndex &, int);

  void on_new_graph_request();

  void reset();

  void set_is_dirty(bool new_is_dirty);

  void show_context_menu(const QPoint &pos);

private:
  void add_list_item(const std::string &id);

private:
  GraphManager *p_graph_manager;

  CoordFrameWidget *coord_frame_widget;

  QListWidget *list_widget;

  QPushButton *apply_button;

  bool is_dirty = false;
};

} // namespace hesiod