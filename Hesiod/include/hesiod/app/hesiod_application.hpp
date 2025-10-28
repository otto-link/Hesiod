/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <memory>

#include <QApplication>
#include <QCoreApplication>
#include <QMainWindow>
#include <QProgressBar>
#include <QStandardPaths>

#include "nlohmann/json.hpp"

#include "hesiod/app/app_context.hpp"
#include "hesiod/gui/widgets/bake_config_dialog.hpp"

#define HSD_CTX                                                                          \
  static_cast<hesiod::HesiodApplication *>(QCoreApplication::instance())->get_context()

namespace hesiod
{

// forward
class ProjectUI;

// =====================================
// HesiodApplication
// =====================================
class HesiodApplication : public QApplication
{
  Q_OBJECT
public:
  HesiodApplication(int &argc, char **argv);
  ~HesiodApplication();

  void load_project_model_and_ui(const std::string &fname = "");
  void save_project_model_and_ui(const std::string &fname);
  void show();

  // --- Context
  QApplication     &get_qapp();
  AppContext       &get_context();
  const AppContext &get_context() const;
  ProjectUI        *get_project_ui_ref();

private slots:
  // --- User actions
  void on_export_batch();
  void on_load();
  void on_new();
  void on_quit();
  void on_save();
  void on_save_as();
  void on_save_copy();
  void show_about();
  void show_quick_help();

  // --- Internal
  void on_project_name_changed();

private:
  void cleanup();
  void setup_menu_bar();

  // --- Members
  AppContext                   context;
  std::unique_ptr<ProjectUI>   project_ui;
  std::unique_ptr<QMainWindow> main_window;
  QProgressBar                *progress_bar; // owned by main_window
};

} // namespace hesiod