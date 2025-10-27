/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <memory>

#include <QApplication>
#include <QCoreApplication>
#include <QMainWindow>
#include <QStandardPaths>

#include "nlohmann/json.hpp"

#include "hesiod/app/app_context.hpp"
#include "hesiod/gui/widgets/bake_and_export_settings_dialog.hpp"

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

  void show();

  // --- Context
  QApplication     &get_qapp();
  AppContext       &get_context();
  const AppContext &get_context() const;

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
  void load_project_model_and_ui(const std::string &fname = "");
  void save_project_model_and_ui(const std::string &fname);
  void setup_menu_bar();

  // --- Members
  AppContext                   context;
  std::unique_ptr<QMainWindow> main_window;
  std::unique_ptr<ProjectUI>   project_ui;
};

// =====================================
// Functions
// =====================================
void override_export_nodes_settings(const std::string           &fname,
                                    const std::filesystem::path &export_path,
                                    uint                         random_seeds_increment,
                                    const BakeSettings          &bake_settings);

} // namespace hesiod