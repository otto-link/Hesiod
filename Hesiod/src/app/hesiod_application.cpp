/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>
#include <iostream>
#include <sstream>

#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QStatusBar>

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/cli/batch_mode.hpp"
#include "hesiod/gui/project_ui.hpp"
#include "hesiod/gui/widgets/bake_config_dialog.hpp"
#include "hesiod/gui/widgets/documentation_popup.hpp"
#include "hesiod/gui/widgets/graph_manager_widget.hpp"
#include "hesiod/gui/widgets/graph_tabs_widget.hpp"
#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph/graph_manager.hpp"
#include "hesiod/model/graph/graph_node.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

HesiodApplication::HesiodApplication(int &argc, char **argv) : QApplication(argc, argv)
{
  Logger::log()->trace("HesiodApplication::HesiodApplication");

  this->context.load_settings();

  // apply style
  this->setWindowIcon(QIcon(this->context.app_settings.global.icon_path.c_str()));
  apply_global_style(this->get_qapp());

  // main window
  this->main_window = std::make_unique<QMainWindow>();
  this->main_window->setGeometry(this->context.app_settings.window.x,
                                 this->context.app_settings.window.y,
                                 this->context.app_settings.window.w,
                                 this->context.app_settings.window.h);

  {
    this->progress_bar = new QProgressBar(this->main_window.get());
    this->progress_bar->setRange(0, 100);
    this->progress_bar->setValue(0);
    this->progress_bar->setTextVisible(false);
    this->progress_bar->setFixedWidth(200); // TODO app settings
    this->main_window->statusBar()->addPermanentWidget(this->progress_bar);
  }

  // (after MainWindow creation)
  this->load_project_model_and_ui();

  // others
  this->setup_menu_bar();
}

HesiodApplication::~HesiodApplication() = default;

void HesiodApplication::cleanup()
{
  Logger::log()->trace("HesiodApplication::cleanup");

  if (this->project_ui)
    this->project_ui->cleanup();

  if (this->context.project_model)
    this->context.project_model->cleanup();
}

AppContext &HesiodApplication::get_context() { return this->context; }

const AppContext &HesiodApplication::get_context() const { return this->context; }

ProjectUI *HesiodApplication::get_project_ui_ref() { return this->project_ui.get(); }

QApplication &HesiodApplication::get_qapp() { return *static_cast<QApplication *>(this); }

void HesiodApplication::load_project_model_and_ui(const std::string &fname)
{
  Logger::log()->trace("HesiodApplication::load_project_model_and_ui: fname [{}]", fname);

  const std::string actual_fname = fname.empty() ? this->context.app_settings.global
                                                       .default_startup_project_file
                                                 : fname;

  this->cleanup();

  // model
  this->context.load_project_model(actual_fname);

  // UI
  this->project_ui = std::make_unique<ProjectUI>();
  this->project_ui->initialize(this->context.project_model.get());
  this->project_ui->load_ui_state(actual_fname);

  this->main_window->setCentralWidget(this->project_ui->get_widget());

  // reset other visibility state
  this->project_ui->get_graph_manager_widget_ref()->setVisible(
      this->context.app_settings.window.show_graph_manager_widget);
  this->project_ui->get_texture_downloader_ref()->setVisible(
      this->context.app_settings.window.show_texture_downloader_widget);

  // ProjectUI -> Project
  this->connect(this->project_ui->get_graph_manager_widget_ref(),
                &GraphManagerWidget::has_changed,
                this->context.project_model.get(),
                &ProjectModel::on_has_changed);

  this->connect(this->project_ui->get_graph_tabs_widget_ref(),
                &GraphTabsWidget::has_changed,
                this->context.project_model.get(),
                &ProjectModel::on_has_changed);

  // Project -> HesiodApplication
  this->connect(this->context.project_model.get(),
                &ProjectModel::project_name_changed,
                this,
                &HesiodApplication::on_project_name_changed);

  this->connect(this->context.project_model.get(),
                &ProjectModel::is_dirty_changed,
                this,
                &HesiodApplication::on_project_name_changed);

  // GraphNode model -> MainWindow
  this->connect(this->context.project_model->get_graph_manager_ref(),
                &GraphManager::update_progress,
                this->progress_bar,
                [this](float progress)
                {
                  if (progress == 0.f || progress == 100.f)
                  {
                    this->progress_bar->setValue(0);
                    this->progress_bar->setTextVisible(false);
                    return;
                  }

                  this->progress_bar->setTextVisible(true);
                  this->progress_bar->setValue(static_cast<int>(progress));
                });

  // rename whether fname is empty or not
  this->context.project_model->set_path(fname);
}

void HesiodApplication::on_export_batch()
{
  Logger::log()->trace("HesiodApplication::on_export_batch");

  // --- retrieve export parameters from user

  BakeConfig bake_settings = this->context.project_model->get_bake_config();

  BakeConfigDialog dialog(this->context.app_settings.node_editor.max_bake_resolution,
                          bake_settings);

  if (dialog.exec() != QDialog::Accepted)
    return;

  bake_settings = dialog.get_bake_settings();

  Logger::log()->trace("MainWindow::on_export_batch: size = {}, nvariants = {}",
                       bake_settings.resolution,
                       bake_settings.nvariants);

  // --- setup export repertory

  // block UI
  QProgressDialog progress(tr("Baking and exporting..."),
                           QString(),
                           0,
                           0,
                           this->main_window.get());
  progress.setWindowModality(Qt::ApplicationModal);
  progress.setCancelButton(nullptr);
  progress.setMinimumDuration(0); // show immediately
  progress.show();
  QCoreApplication::processEvents();

  for (int k = 0; k < bake_settings.nvariants + 1; ++k)
  {
    QCoreApplication::processEvents(); // render progress dialog

    const std::filesystem::path project_path = this->context.project_model->get_path();

    // build export path based on project name, if available
    std::filesystem::path export_path = project_path.filename();
    if (export_path.empty())
      export_path = "export";
    else
      export_path += "_export";

    export_path = project_path.empty() ? export_path
                                       : project_path.parent_path() / export_path;

    if (k > 0)
      export_path /= "variants_" + std::to_string(k);

    Logger::log()->info("MainWindow::on_export_batch: export path: {}",
                        export_path.string());

    // create it
    if (!std::filesystem::exists(export_path))
    {
      Logger::log()->trace("MainWindow::on_export_batch: creating export repertory {}",
                           export_path.string());
      std::filesystem::create_directories(export_path);
    }

    // --- save an hesiod file and tweak it

    // save graph node to a temporary file
    std::filesystem::path fname = export_path / "hesiod_bake.hsd";
    this->save_project_model_and_ui(fname.string());

    // force auto_export for export nodes and overwrite export paths
    override_export_nodes_settings(fname.string(),
                                   export_path,
                                   static_cast<uint>(k),
                                   bake_settings);

    // --- run

    // retrieve config of the first graph
    auto graph_nodes = this->context.project_model->get_graph_manager_ref()
                           ->get_graph_nodes();
    auto         it = graph_nodes.begin();
    GraphConfig *p_config = it != graph_nodes.end() ? it->second->get_config_ref()
                                                    : nullptr;

    if (p_config)
    {
      GraphConfig bake_config = *p_config;

      if (bake_settings.force_distributed)
      {
        bake_config.hmap_transform_mode_cpu = hmap::TransformMode::DISTRIBUTED;
        bake_config.hmap_transform_mode_gpu = hmap::TransformMode::DISTRIBUTED;
      }

      // run batch node
      hesiod::cli::run_batch_mode(
          fname.string(),
          hmap::Vec2<int>(bake_settings.resolution, bake_settings.resolution),
          bake_config.tiling,
          bake_config.overlap,
          &bake_config);
    }
  }

  // save config
  this->context.project_model->set_bake_config(bake_settings);

  // unblock UI
  progress.close();
}

void HesiodApplication::on_load()
{
  Logger::log()->trace("HesiodApplication::on_load");

  std::filesystem::path path = this->context.project_model->get_path();

  QString load_fname = QFileDialog::getOpenFileName(this->main_window.get(),
                                                    "Load...",
                                                    path.string().c_str(),
                                                    "Hesiod files (*.hsd)");

  if (!load_fname.isNull() && !load_fname.isEmpty())
    this->load_project_model_and_ui(load_fname.toStdString());
}

void HesiodApplication::on_new()
{
  Logger::log()->trace("HesiodApplication::on_new");

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(nullptr,
                                "New",
                                "Clear everything, are you sure?",
                                QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes)
  {
    this->cleanup();
    this->load_project_model_and_ui();
  }
}

void HesiodApplication::on_project_name_changed()
{
  std::string title = this->context.project_model->get_name() + " [" +
                      this->context.project_model->get_path().string() + "]";

  if (this->context.project_model->get_is_dirty())
    title += "*";

  this->main_window->setWindowTitle(title.c_str());
}

void HesiodApplication::on_quit()
{
  Logger::log()->trace("HesiodApplication::on_quit");

  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(nullptr,
                                "Quit",
                                "Quitting the application, are you sure?",
                                QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes)
  {
    QApplication::quit();

    // save main window geometry
    QRect geom = this->main_window->geometry();
    this->context.app_settings.window.x = geom.x();
    this->context.app_settings.window.y = geom.y();
    this->context.app_settings.window.w = geom.width();
    this->context.app_settings.window.h = geom.height();

    this->context.save_settings();
  }
}

void HesiodApplication::on_save()
{
  Logger::log()->trace("HesiodApplication::on_save");

  std::filesystem::path path = this->context.project_model->get_path();

  if (path.empty())
    this->on_save_as();
  else
  {
    this->save_project_model_and_ui(path.string());
    this->context.project_model->set_path(path);
  }
}

void HesiodApplication::on_save_as()
{
  Logger::log()->trace("HesiodApplication::on_save_as");

  std::filesystem::path path = this->context.project_model->get_path();

  QString new_fname = QFileDialog::getSaveFileName(this->main_window.get(),
                                                   "Save as...",
                                                   path.string().c_str(),
                                                   "Hesiod files (*.hsd)");

  if (!new_fname.isNull() && !new_fname.isEmpty())
  {
    this->save_project_model_and_ui(new_fname.toStdString());
    this->context.project_model->set_path(new_fname.toStdString());
  }
}

void HesiodApplication::on_save_copy()
{
  Logger::log()->trace("HesiodApplication::on_save_copy");

  std::filesystem::path path = this->context.project_model->get_path();

  Logger::log()->trace("{}", path.string());

  if (path.empty())
    this->on_save_as();
  else
  {
    std::filesystem::path fname = insert_before_extension(path, "_" + time_stamp());
    this->save_project_model_and_ui(fname.string());
  }
}

void HesiodApplication::save_project_model_and_ui(const std::string &fname)
{
  Logger::log()->trace("HesiodApplication::save_project_model_and_ui: {}", fname);

  this->context.save_project_model(fname);
  this->context.project_model->set_is_dirty(false);

  this->project_ui->save_ui_state(fname);

  // add some global info
  nlohmann::json json;
  json["Hesiod version"] = "v" + std::to_string(HESIOD_VERSION_MAJOR) + "." +
                           std::to_string(HESIOD_VERSION_MINOR) + "." +
                           std::to_string(HESIOD_VERSION_PATCH);

  json["saved_at"] = time_stamp();
  json_to_file(json, fname, /* merge_with_existing_content */ true);
}

void HesiodApplication::setup_menu_bar()
{
  Logger::log()->trace("HesiodApplication::setup_menu_bar");

  // --- create menu
  QMenu *file_menu = this->main_window->menuBar()->addMenu("&File");

  auto *new_action = new QAction("New", this);
  new_action->setShortcut(tr("Ctrl+N"));
  file_menu->addAction(new_action);

  file_menu->addSeparator();

  auto *load_action = new QAction("Open", this);
  load_action->setShortcut(tr("Ctrl+O"));
  file_menu->addAction(load_action);

  auto *save = new QAction("Save", this);
  save->setShortcut(tr("Ctrl+S"));
  file_menu->addAction(save);

  auto *save_as = new QAction("Save As", this);
  save_as->setShortcut(tr("Ctrl+Shift+S"));
  file_menu->addAction(save_as);

  auto *save_copy = new QAction("Save a copy", this);
  save_copy->setShortcut(tr("Ctrl+Alt+S"));
  file_menu->addAction(save_copy);

  file_menu->addSeparator();

  auto *export_batch = new QAction("Bake and Export (High Resolution)", this);
  export_batch->setShortcut(tr("Alt+E"));
  file_menu->addAction(export_batch);

  file_menu->addSeparator();

  auto *quit = new QAction("Quit", this);
  quit->setShortcut(tr("Ctrl+Q"));
  file_menu->addAction(quit);

  QMenu *graph_menu = this->main_window->menuBar()->addMenu("&Graph");

  auto *new_graph = new QAction("New graph", this);
  graph_menu->addAction(new_graph);

  graph_menu->addSeparator();

  auto *reseed = new QAction("Advance random seeds", this);
  reseed->setShortcut(tr("Alt+R"));
  graph_menu->addAction(reseed);

  auto *reseed_back = new QAction("Reverse random seeds", this);
  reseed_back->setShortcut(tr("Alt+Shift+R"));
  graph_menu->addAction(reseed_back);

  QMenu *view_menu = this->main_window->menuBar()->addMenu("&View");

  auto *show_layout_manager = new QAction("Graph layout manager", this);
  show_layout_manager->setCheckable(true);
  show_layout_manager->setChecked(
      this->context.app_settings.window.show_graph_manager_widget);
  view_menu->addAction(show_layout_manager);

  view_menu->addSeparator();

  auto *show_texture_downloader = new QAction("Texture downloader", this);
  show_texture_downloader->setCheckable(true);
  show_texture_downloader->setChecked(
      this->context.app_settings.window.show_texture_downloader_widget);
  view_menu->addAction(show_texture_downloader);

  view_menu->addSeparator();

  auto *show_node_settings_pan_action = new QAction("Show node settings pan", this);
  show_node_settings_pan_action->setCheckable(true);
  {
    bool state = this->context.app_settings.node_editor.show_node_settings_pan;
    show_node_settings_pan_action->setChecked(state);
    this->project_ui->get_graph_tabs_widget_ref()->set_show_node_settings_widget(state);
    view_menu->addAction(show_node_settings_pan_action);
  }

  QMenu *help = this->main_window->menuBar()->addMenu("&Help");

  auto *quick_help = new QAction("Quick help", this);
  help->addAction(quick_help);

  help->addSeparator();

  auto *about = new QAction("&About", this);
  help->addAction(about);

  // --- connections

  this->connect(new_action, &QAction::triggered, this, &HesiodApplication::on_new);
  this->connect(load_action, &QAction::triggered, this, &HesiodApplication::on_load);

  this->connect(save, &QAction::triggered, this, &HesiodApplication::on_save);
  this->connect(save_as, &QAction::triggered, this, &HesiodApplication::on_save_as);
  this->connect(save_copy, &QAction::triggered, this, &HesiodApplication::on_save_copy);
  this->connect(export_batch,
                &QAction::triggered,
                this,
                &HesiodApplication::on_export_batch);

  this->connect(quick_help,
                &QAction::triggered,
                this,
                &HesiodApplication::show_quick_help);
  this->connect(about, &QAction::triggered, this, &HesiodApplication::show_about);

  // quit
  this->connect(quit, &QAction::triggered, this, &HesiodApplication::on_quit);

  // satellite widgets

  this->connect(
      show_node_settings_pan_action,
      &QAction::triggered,
      this,
      [this, show_node_settings_pan_action]()
      {
        bool new_state = !this->context.app_settings.node_editor.show_node_settings_pan;
        this->context.app_settings.node_editor.show_node_settings_pan = new_state;
        show_node_settings_pan_action->setChecked(new_state);
        this->project_ui->get_graph_tabs_widget_ref()->set_show_node_settings_widget(
            new_state);
      });

  this->connect(
      show_layout_manager,
      &QAction::triggered,
      this,
      [this, show_layout_manager]()
      {
        bool state = this->project_ui->get_graph_manager_widget_ref()->isVisible();
        this->context.app_settings.window.show_graph_manager_widget = !state;
        this->project_ui->get_graph_manager_widget_ref()->setVisible(!state);
        show_layout_manager->setChecked(!state);
      });

  this->connect(
      show_texture_downloader,
      &QAction::triggered,
      this,
      [this, show_texture_downloader]()
      {
        bool state = this->project_ui->get_texture_downloader_ref()->isVisible();
        this->context.app_settings.window.show_texture_downloader_widget = !state;
        this->project_ui->get_texture_downloader_ref()->setVisible(!state);
        show_texture_downloader->setChecked(!state);
      });

  // graphs
  this->connect(
      new_graph,
      &QAction::triggered,
      this,
      [this]()
      { this->project_ui->get_graph_manager_widget_ref()->on_new_graph_request(); });

  this->connect(reseed,
                &QAction::triggered,
                this,
                [this]()
                { this->project_ui->get_graph_manager_widget_ref()->on_reseed(); });

  this->connect(reseed_back,
                &QAction::triggered,
                this,
                [this]()
                { this->project_ui->get_graph_manager_widget_ref()->on_reseed(true); });
}

void HesiodApplication::show()
{
  Logger::log()->trace("HesiodApplication::show");
  this->main_window->show();
}

void HesiodApplication::show_about()
{
  std::string msg = "Hesiod v";
  msg += std::to_string(HESIOD_VERSION_MAJOR) + ".";
  msg += std::to_string(HESIOD_VERSION_MINOR) + ".";
  msg += std::to_string(HESIOD_VERSION_PATCH) + "\n";
  msg += "A desktop application for node-based "
         "procedural terrain "
         "generation.\nhttps://github.com/otto-link/Hesiod";

  QMessageBox msg_box;
  msg_box.setText(msg.c_str());
  QPixmap icon(this->context.app_settings.global.icon_path.c_str());
  msg_box.setIconPixmap(
      icon.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

  msg_box.exec();
}

void HesiodApplication::show_quick_help()
{
  Logger::log()->trace("HesiodApplication::show_quick_help");

  std::string html_source = "";

  std::ifstream file(this->context.app_settings.global.quick_start_html_file);
  if (file.is_open())
  {
    std::ostringstream buffer;
    buffer << file.rdbuf(); // read the entire file into the buffer
    html_source = buffer.str();
    file.close();
  }

  DocumentationPopup *popup = new DocumentationPopup("Hesiod Quick Help", html_source);

  popup->setAttribute(Qt::WA_DeleteOnClose);
  popup->show();
}

} // namespace hesiod
