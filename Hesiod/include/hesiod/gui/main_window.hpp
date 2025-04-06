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

#define HSD_SETTINGS_ORG "olink"
#define HSD_SETTINGS_APP "hesiod"
#define HSD_DEFAULT_STARTUP_FILE "data/default.hsd"
#define HSD_APP_ICON "data/hesiod_icon.png"

namespace hesiod
{

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  // static method to get the singleton instance of MainWindow
  static MainWindow *instance(QApplication *p_app = nullptr, QWidget *p_parent = nullptr)
  {
    static MainWindow *instance = nullptr;
    if (!instance)
    {
      instance = new MainWindow(p_app, p_parent);
    }
    return instance;
  }

  void load_from_file(const std::string &fname);

  std::string get_project_name() const;

  bool save_to_file(const std::string &fname) const;

  void set_project_path(const std::string &new_project_path);

  void set_project_path(const std::filesystem::path &new_project_path);

  void set_title(const std::string &new_title)
  {
    this->setWindowTitle(new_title.c_str());
  }

protected:
  void restore_state();

  void save_state();

  void show_about();

private Q_SLOTS:
  void closeEvent(QCloseEvent *event) override;

  void on_load();

  void on_new();

  void on_quit();

private: // methods
  // constructor is private to prevent creating MainWindow elsewhere
  MainWindow(QApplication *p_app = nullptr, QWidget *p_parent = nullptr);

  ~MainWindow() override;

  // prevent copying and assignment
  MainWindow(const MainWindow &) = delete;

  MainWindow &operator=(const MainWindow &) = delete;

  void setup_central_widget();

  void setup_graph_manager();

  void setup_menu_bar();

private: // attributes
  std::filesystem::path project_path = "";

  // graph-related storage
  std::unique_ptr<hesiod::GraphManager> graph_manager;

  // ownership by MainWindow Qt object
  GraphManagerWidget *graph_manager_widget = nullptr;

  std::vector<GraphEditor *> graph_editors = {};

  QTabWidget *tab_widget = nullptr;
};

} // namespace hesiod