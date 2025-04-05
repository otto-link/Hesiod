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
#include <QComboBox>
#include <QListWidget>
#include <QWidget>

#include "hesiod/graph_editor.hpp"
#include "hesiod/graph_manager.hpp"
#include "hesiod/gui/widgets/coord_frame_widget.hpp"

#define LIST_ITEM_HEIGHT 64

namespace hesiod
{

class GraphQListWidget : public QWidget
{
  Q_OBJECT

public:
  GraphQListWidget(GraphEditor *p_graph_editor, QWidget *parent = nullptr);

  void json_from(nlohmann::json const &json);

  nlohmann::json json_to() const;

Q_SIGNALS:
  void bg_image_updated(const std::string &graph_id, const QImage &image);

public Q_SLOTS:
  void update_combobox();

  void on_combobox_changed();

private:
  GraphEditor *p_graph_editor;

  QComboBox *combobox;

  std::string current_bg_tag;
};

class GraphManagerWidget : public QWidget
{
  Q_OBJECT

public:
  GraphManagerWidget(GraphManager *p_graph_manager, QWidget *parent = nullptr);

  void json_from(nlohmann::json const &json);

  nlohmann::json json_to() const;

  void restore_window_state();

  void save_window_state() const;

private Q_SLOTS:
  void closeEvent(QCloseEvent *event) override;

  void hideEvent(QHideEvent *event) override;

  void on_apply_changes();

  void on_item_double_clicked(QListWidgetItem *item);

  void on_list_reordered(const QModelIndex &, int, int, const QModelIndex &, int);

  void on_new_graph_request();

  void on_nodes_ref_updated();

  void reset();

  void set_is_dirty(bool new_is_dirty);

  void show_context_menu(const QPoint &pos);

  void showEvent(QShowEvent *event) override;

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