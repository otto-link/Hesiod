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
#include <QAction>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>

#include "nlohmann/json.hpp"

#include "hesiod/gui/widgets/coord_frame_widget.hpp"
#include "hesiod/model/broadcast_param.hpp"

#define LIST_ITEM_HEIGHT 64

namespace hesiod
{

// forward
class GraphEditor;
class GraphManager;
class GraphManagerWidget;

// =====================================
// GraphQListWidget
// =====================================
class GraphQListWidget : public QWidget
{
  Q_OBJECT

public:
  GraphQListWidget(GraphEditor *p_graph_editor, QWidget *parent = nullptr);

  // --- Serialization ---
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

signals:
  void bg_image_updated(const std::string &graph_id, const QImage &image);

public slots:
  // --- User Actions ---
  void on_combobox_changed();

  // --- GUI ---
  void update_combobox();

private:
  // --- Members ---
  GraphEditor *p_graph_editor;
  QComboBox   *combobox;
  std::string  current_bg_tag;
};

// =====================================
// GraphManagerWidget
// =====================================
class GraphManagerWidget : public QWidget
{
  Q_OBJECT

public:
  GraphManagerWidget(GraphManager *p_graph_manager, QWidget *parent = nullptr);

  // --- Serialization ---
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

  // --- State Management ---
  void restore_window_state();
  void save_window_state() const;

signals:
  void window_closed();

public slots:
  // --- User Actions ---
  void on_apply_changes();
  void on_item_double_clicked(QListWidgetItem *item);
  void on_list_reordered(const QModelIndex &, int, int, const QModelIndex &, int);
  void on_new_graph_request();

private slots:
  // --- Qt Events ---
  void closeEvent(QCloseEvent *event) override;
  void hideEvent(QHideEvent *event) override;
  void showEvent(QShowEvent *event) override;

  // --- Internal State ---
  void reset();
  void set_is_dirty(bool new_is_dirty);

  // --- GUI ---
  void show_context_menu(const QPoint &pos);

private:
  // --- Helper(s) ---
  void add_list_item(const std::string &id);

  // --- Members ---
  GraphManager     *p_graph_manager;
  CoordFrameWidget *coord_frame_widget;
  QListWidget      *list_widget;
  QPushButton      *apply_button;
  bool              is_dirty = false;
};

} // namespace hesiod