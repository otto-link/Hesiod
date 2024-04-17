/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QMainWindow>

#include "hesiod/gui/node_editor_widget.hpp"

namespace hesiod
{

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QApplication *p_app = nullptr, QWidget *p_parent = nullptr);

  ~MainWindow() override;

protected:
  void restore_state();

  void save_state();

  void show_about();

private Q_SLOTS:
  void on_load();

  void on_new();

  void on_quit();

  void on_save();

private:
  void closeEvent(QCloseEvent *event) override;

  std::vector<NodeEditorWidget *> node_editor_widgets;
};

} // namespace hesiod
