/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file main_window.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <QMainWindow>

#include "hesiod/graph_editor.hpp"

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
  void on_quit();

private:
  void closeEvent(QCloseEvent *event) override;

  std::vector<GraphEditor *> graph_editors = {};
};

} // namespace hesiod