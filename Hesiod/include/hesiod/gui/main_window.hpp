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
#include <filesystem>

#include <QMainWindow>

#define HSD_SETTINGS_ORG "olink"
#define HSD_SETTINGS_APP "hesiod"
#define HSD_DEFAULT_STARTUP_FILE "data/default.hsd"
#define HSD_APP_ICON "data/hesiod_icon.png"

// TODO DBG
#include "hesiod/gui/widgets/graph_node_widget.hpp"

namespace hesiod
{

// forward
class GraphManagerWidget;
class GraphTabsWidget;

class GraphManager;

// =====================================
// FrameItem
// =====================================
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  // --- Singleton pattern ---
  static MainWindow *instance(QApplication *p_app = nullptr, QWidget *p_parent = nullptr);

  void clear_all(); // model + GUI

  // --- Accessors ---
  std::string get_project_name() const;
  void        set_is_dirty(bool new_is_dirty);
  void        set_project_path(const std::string &new_project_path);
  void        set_project_path(const std::filesystem::path &new_project_path);
  void        set_title(const std::string &new_title);

  // --- Serialization on file ---
  void load_from_file(const std::string &fname);
  bool save_to_file(const std::string &fname) const;

protected:
  // --- State Management ---
  void restore_state();
  void save_state();

private slots:
  // --- Event Handling ---
  void closeEvent(QCloseEvent *event) override;

  // --- User Actions ---
  void on_autosave();
  void on_has_changed(); // whole project
  void on_load();
  void on_new();
  void on_quit();
  void on_save();
  void on_save_as();
  void on_save_copy();
  void show_about();

private:
  // --- Singleton pattern (private constructor to prevent copy) ---
  MainWindow(QApplication *p_app = nullptr, QWidget *p_parent = nullptr);
  ~MainWindow() override;

  // prevent copying and assignment
  MainWindow(const MainWindow &) = delete;
  MainWindow &operator=(const MainWindow &) = delete;

  // --- Setup ---
  void setup_central_widget();
  void setup_graph_manager();
  void setup_menu_bar();
  void update_window_title();

  // --- Storage... ---
  std::unique_ptr<GraphManager>       graph_manager;
  std::unique_ptr<GraphManagerWidget> graph_manager_widget;
  std::unique_ptr<GraphTabsWidget>    graph_tabs_widget;

  // --- Members ---
  std::filesystem::path project_path = "";
  bool                  is_dirty = false;
  QTimer               *autosave_timer; // own by this
};

// =====================================
// Functions
// =====================================

void notify(const std::string &title, const std::string &text);

} // namespace hesiod