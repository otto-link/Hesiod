/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

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
#include "hesiod/graph_manager.hpp"
#include "hesiod/gui/widgets/graph_manager_widget.hpp"

namespace hesiod
{

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  // Static method to get the singleton instance of MainWindow
  static MainWindow *instance(QApplication *p_app = nullptr, QWidget *p_parent = nullptr)
  {
    static MainWindow *instance = nullptr;
    if (!instance)
    {
      instance = new MainWindow(p_app, p_parent);
    }
    return instance;
  }

  static bool exists() { return instance() != nullptr; }

  // Method to change the window title
  void set_title(const std::string &new_title)
  {
    this->setWindowTitle(new_title.c_str());
  }

protected:
  void restore_state();

  void save_state();

  void show_about();

private Q_SLOTS:
  void on_quit();

private:
  // Constructor is private to prevent creating MainWindow elsewhere
  MainWindow(QApplication *p_app = nullptr, QWidget *p_parent = nullptr);

  ~MainWindow() override;

  // Prevent copying and assignment
  MainWindow(const MainWindow &) = delete;

  MainWindow &operator=(const MainWindow &) = delete;

  void closeEvent(QCloseEvent *event) override;

  // graph-related storage
  std::unique_ptr<hesiod::GraphManager> graph_manager;

  // ownership by MainWindow
  GraphManagerWidget *graph_manager_widget = nullptr;

  std::vector<GraphEditor *> graph_editors = {};
};

} // namespace hesiod