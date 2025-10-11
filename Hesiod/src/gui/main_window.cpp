/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>
#include <QTabWidget>
#include <QTimer>

#include "Toast.h"

#include "hesiod/cli/batch_mode.hpp"
#include "hesiod/config.hpp"
#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/gui/main_window.hpp"
#include "hesiod/gui/widgets/bake_and_export_settings_dialog.hpp"
#include "hesiod/gui/widgets/documentation_popup.hpp"
#include "hesiod/gui/widgets/graph_manager_widget.hpp"
#include "hesiod/gui/widgets/graph_tabs_widget.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_manager.hpp"
#include "hesiod/model/graph_node.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

MainWindow::MainWindow(QApplication *p_app, QWidget *parent) : QMainWindow(parent)
{
  this->restore_state();

  // model
  this->graph_manager = std::make_unique<GraphManager>();

  // GUI
  this->graph_manager_widget = std::make_unique<GraphManagerWidget>(
      this->graph_manager.get());

  if (HSD_CONFIG->window.open_graph_manager_at_startup)
    this->graph_manager_widget->show();

  this->graph_tabs_widget = std::make_unique<GraphTabsWidget>(this->graph_manager.get());

  this->setup_central_widget();

  // --- connections

  // widgets -> MainWindow
  this->connect(this->graph_manager_widget.get(),
                &GraphManagerWidget::has_changed,
                this,
                &MainWindow::on_has_changed);

  this->connect(this->graph_tabs_widget.get(),
                &GraphTabsWidget::has_changed,
                this,
                &MainWindow::on_has_changed);

  // GraphManagerWidget -> GraphTabsWidget
  this->connect(this->graph_manager_widget.get(),
                &GraphManagerWidget::graph_removed,
                this->graph_tabs_widget.get(),
                &GraphTabsWidget::update_tab_widget);

  this->connect(this->graph_manager_widget.get(),
                &GraphManagerWidget::list_reordered,
                this->graph_tabs_widget.get(),
                &GraphTabsWidget::update_tab_widget);

  this->connect(this->graph_manager_widget.get(),
                &GraphManagerWidget::new_graph_added,
                this->graph_tabs_widget.get(),
                &GraphTabsWidget::update_tab_widget);

  this->connect(this->graph_manager_widget.get(),
                &GraphManagerWidget::selected_graph_changed,
                this->graph_tabs_widget.get(),
                &GraphTabsWidget::set_selected_tab);

  // GraphTabsWidget -> GraphManagerWidget
  this->connect(this->graph_tabs_widget.get(),
                &GraphTabsWidget::has_been_cleared,
                this->graph_manager_widget.get(),
                &GraphManagerWidget::update_combobox);

  this->connect(this->graph_tabs_widget.get(),
                &GraphTabsWidget::new_node_created,
                this,
                [this](const std::string &graph_id, const std::string & /* id */)
                { this->graph_manager_widget->update_combobox(graph_id); });

  this->connect(this->graph_tabs_widget.get(),
                &GraphTabsWidget::node_deleted,
                this,
                [this](const std::string &graph_id, const std::string & /* id */)
                { this->graph_manager_widget->update_combobox(graph_id); });

  // qtd::TextureDownloader -> GraphTabsWidget

  this->connect(&this->texture_downloader,
                &qtd::TextureDownloader::textures_retrieved,
                this->graph_tabs_widget.get(),
                &GraphTabsWidget::on_textures_request);

  // TODO connect "get"

  // MainWindow -> MainWindow
  this->autosave_timer = new QTimer(this);
  this->autosave_timer->setSingleShot(true); // restarts on project change
  this->autosave_timer->start(HSD_CONFIG->window.autosave_timer);

  this->connect(this->autosave_timer, &QTimer::timeout, this, &MainWindow::on_autosave);

  // QApp -> MainWindow
  this->connect(p_app, &QApplication::aboutToQuit, [&]() { this->save_state(); });

  // --- finish setup

  // after widgets and graph manager
  this->setup_menu_bar();

  // load default config
  this->load_from_file(HSD_DEFAULT_STARTUP_FILE);
  this->graph_tabs_widget->zoom_to_content();

  // here, at the end
  this->set_is_dirty(false);
}

MainWindow::~MainWindow() { this->save_state(); }

void MainWindow::clear_all()
{
  this->set_project_path(std::filesystem::path());

  // model
  this->graph_manager->clear();

  // GUI
  this->graph_manager_widget->clear();
  this->graph_tabs_widget->clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  this->save_state();
  event->accept();
}

GraphTabsWidget *MainWindow::graph_tabs_widget_ref()
{
  return this->graph_tabs_widget.get();
}

std::string MainWindow::get_project_name() const
{
  if (this->project_path.empty())
    return std::string();
  else
    return this->project_path.filename().string();
}

MainWindow *MainWindow::instance(QApplication *p_app, QWidget *p_parent)
{
  static MainWindow *instance = nullptr;
  if (!instance)
  {
    instance = new MainWindow(p_app, p_parent);
  }
  return instance;
}

void MainWindow::json_from(nlohmann::json const &json)
{
  Logger::log()->trace("MainWindow::json_from");

  json_safe_get(json, "show_node_settings_pan", this->show_node_settings_pan);
}

nlohmann::json MainWindow::json_to() const
{
  Logger::log()->trace("MainWindow::json_to");

  nlohmann::json json;
  json["show_node_settings_pan"] = this->show_node_settings_pan;

  return json;
}

void MainWindow::load_from_file(const std::string &fname)
{
  Logger::log()->trace("MainWindow::load_from_file: {}", fname);

  nlohmann::json json = json_from_file(fname);

  this->clear_all();

  // model
  this->graph_manager->json_from(json["graph_manager"]);
  this->graph_manager->update();

  // GUI
  this->graph_manager_widget->json_from(json["graph_manager_widget"]);
  this->graph_tabs_widget->json_from(json["graph_tabs_widget"]);
}

void MainWindow::on_autosave()
{
  std::filesystem::path fname = this->project_path.empty() ? "./no_name.hsd"
                                                           : this->project_path;

  fname = insert_before_extension(fname, "_autosave");
  this->save_to_file(fname.string());
}

void MainWindow::on_export_batch()
{
  Logger::log()->trace("MainWindow::on_export_batch");

  // --- retrieve export parameters from user

  BakeAndExportSettingsDialog dialog(8192 * 4,
                                     this->current_bake_resolution,
                                     this->current_bake_nvariants,
                                     this->current_bake_force_distributed);

  if (dialog.exec() != QDialog::Accepted)
    return;

  int  size = dialog.get_size();
  int  nvariants = dialog.get_nvariants();
  bool force_distributed = dialog.get_force_distributed();

  this->current_bake_resolution = size;
  this->current_bake_nvariants = nvariants;
  this->current_bake_force_distributed = force_distributed;

  Logger::log()->trace("MainWindow::on_export_batch: size = {}, nvariants = {}");

  // --- setup export repertory

  // block UI
  QProgressDialog progress(tr("Baking and exporting..."), QString(), 0, 0, this);
  progress.setWindowModality(Qt::ApplicationModal);
  progress.setCancelButton(nullptr);
  progress.setMinimumDuration(0); // show immediately
  progress.show();
  QCoreApplication::processEvents();

  for (int k = 0; k < nvariants + 1; ++k)
  {
    QCoreApplication::processEvents(); // render progress dialog

    // build export path based on project name, if available
    std::filesystem::path export_path = this->project_path.filename();
    if (export_path.empty())
      export_path = "export";
    else
      export_path += "_export";

    export_path = this->project_path.empty()
                      ? export_path
                      : this->project_path.parent_path() / export_path;

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
    bool                  ret = this->save_to_file(fname.string(), /*save_backup*/ false);

    if (!ret)
    {
      Logger::log()->error(
          "MainWindow::on_export_batch: error while saving hsd temporary file: {}",
          fname.string());
      notify("Bake & Export", "Error while saving temporary hsd file!");
    }

    // force auto_export for export nodes and overwrite export paths
    override_export_nodes_settings(fname.string(), export_path, static_cast<uint>(k));

    // --- run

    // retrieve config of the first graph (we only need the CPU and GPU compute modes)
    auto         graph_nodes = this->graph_manager->get_graph_nodes();
    auto         it = graph_nodes.begin();
    ModelConfig *p_config = it != graph_nodes.end() ? it->second->get_config_ref()
                                                    : nullptr;

    if (p_config)
    {
      ModelConfig bake_config = *p_config;

      if (force_distributed)
      {
        bake_config.hmap_transform_mode_cpu = hmap::TransformMode::DISTRIBUTED;
        bake_config.hmap_transform_mode_gpu = hmap::TransformMode::DISTRIBUTED;
      }

      // run batch node
      hesiod::cli::run_batch_mode(fname.string(),
                                  hmap::Vec2<int>(size, size),
                                  bake_config.tiling,
                                  bake_config.overlap,
                                  &bake_config);
    }
  }

  // unblock UI
  progress.close();
}

void MainWindow::on_has_changed()
{
  Logger::log()->trace("MainWindow::on_has_changed");
  this->set_is_dirty(true);
  this->autosave_timer->start();
}

void MainWindow::on_load()
{
  std::filesystem::path path = this->project_path.parent_path();

  QString load_fname = QFileDialog::getOpenFileName(this,
                                                    "Load...",
                                                    path.string().c_str(),
                                                    "Hesiod files (*.hsd)");

  if (!load_fname.isNull() && !load_fname.isEmpty())
  {
    this->load_from_file(load_fname.toStdString());
    this->set_project_path(load_fname.toStdString());
  }
}

void MainWindow::on_new()
{
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(nullptr,
                                "New",
                                "Clear everything, are you sure?",
                                QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes)
    this->clear_all();
}

void MainWindow::on_quit()
{
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(nullptr,
                                "Quit",
                                "Quitting the application, are you sure?",
                                QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes)
    QApplication::quit();
}

void MainWindow::on_save()
{
  if (this->project_path.empty())
    this->on_save_as();
  else
  {
    this->save_to_file(this->project_path.string());
    this->set_is_dirty(false);

    notify("Save", std::format("Saved file: {}", this->project_path.string()));
  }
}

void MainWindow::on_save_as()
{
  std::filesystem::path path = this->project_path.parent_path();

  QString new_fname = QFileDialog::getSaveFileName(this,
                                                   "Save as...",
                                                   path.string().c_str(),
                                                   "Hesiod files (*.hsd)");

  if (!new_fname.isNull() && !new_fname.isEmpty())
  {
    this->set_project_path(new_fname.toStdString());
    this->save_to_file(new_fname.toStdString());
    this->set_is_dirty(false);
  }
}

void MainWindow::on_save_copy()
{
  if (this->project_path.empty())
    this->on_save_as();
  else
  {
    std::filesystem::path fname = insert_before_extension(this->project_path,
                                                          "_" + time_stamp());
    this->save_to_file(fname.string());
    this->set_is_dirty(false);

    notify("Save a copy",
           std::format("A copy of the current project has been saved as: {}",
                       fname.filename().string()));
  }
}

void MainWindow::restore_state()
{
  Logger::log()->trace("MainWindow::restore_state");

  // Qt part
  QSettings settings(HSD_SETTINGS_ORG, HSD_SETTINGS_APP);
  this->restoreState(settings.value("MainWindow/state").toByteArray());
  this->restoreGeometry(settings.value("MainWindow/geometry").toByteArray());

  // all the rest...
  nlohmann::json json = json_from_file(HSD_SETTINGS_JSON);
  if (!json.is_null())
    this->json_from(json);
}

void MainWindow::save_state()
{
  Logger::log()->trace("MainWindow::save_state");

  // Qt
  QSettings settings(HSD_SETTINGS_ORG, HSD_SETTINGS_APP);
  settings.setValue("MainWindow/state", this->saveState());
  settings.setValue("MainWindow/geometry", this->saveGeometry());

  // all the rest...
  json_to_file(this->json_to(), HSD_SETTINGS_JSON);
}

bool MainWindow::save_to_file(const std::string &fname, bool save_backup_file) const
{
  Logger::log()->trace("MainWindow::save_to_file: {}", fname);

  const std::string fname_ext = ensure_extension(fname, ".hsd").string();

  try
  {
    if (save_backup_file)
    {
      // If the file exists, create a backup before overwriting
      if (std::filesystem::exists(fname_ext) && HSD_CONFIG->window.save_backup_file)
      {
        std::filesystem::path original_path(fname_ext);
        std::filesystem::path backup_path = insert_before_extension(original_path,
                                                                    ".bak");

        try
        {
          std::filesystem::copy_file(original_path,
                                     backup_path,
                                     std::filesystem::copy_options::overwrite_existing);
          Logger::log()->trace("Backup created: {}", backup_path.string());
        }
        catch (const std::exception &e)
        {
          Logger::log()->warn("Failed to create backup for {}: {}", fname_ext, e.what());
        }
      }
    }

    nlohmann::json json;

    // general infos
    json["Hesiod version"] = "v" + std::to_string(HESIOD_VERSION_MAJOR) + "." +
                             std::to_string(HESIOD_VERSION_MINOR) + "." +
                             std::to_string(HESIOD_VERSION_PATCH);

    json["saved_at"] = time_stamp();

    // model
    json["graph_manager"] = this->graph_manager->json_to();

    // GUI
    json["graph_manager_widget"] = this->graph_manager_widget->json_to();
    json["graph_tabs_widget"] = this->graph_tabs_widget->json_to();

    json_to_file(json, fname_ext);
    return true;
  }
  catch (const std::exception &e)
  {
    Logger::log()->critical("MainWindow::save_to_file: failed to save file {}, what: {}",
                            fname_ext,
                            e.what());
    return false;
  }
}

void MainWindow::set_is_dirty(bool new_is_dirty)
{
  this->is_dirty = new_is_dirty;
  this->update_window_title();
}

void MainWindow::set_project_path(const std::string &new_project_path)
{
  this->set_project_path(std::filesystem::path(new_project_path));
}

void MainWindow::set_project_path(const std::filesystem::path &new_project_path)
{
  this->project_path = new_project_path;
  this->update_window_title();
}

void MainWindow::set_title(const std::string &new_title)
{
  this->setWindowTitle(new_title.c_str());
}

void MainWindow::setup_central_widget()
{
  QWidget *central_widget = new QWidget(this);

  QHBoxLayout *main_layout = new QHBoxLayout(central_widget);
  main_layout->addWidget(this->graph_tabs_widget.get());
  central_widget->setLayout(main_layout);

  this->setCentralWidget(central_widget);
}

void MainWindow::setup_graph_manager()
{
  // TODO default graph
}

void MainWindow::setup_menu_bar()
{
  QMenu *file_menu = menuBar()->addMenu("&File");

  auto *new_action = new QAction("New", this);
  new_action->setShortcut(tr("Ctrl+N"));
  file_menu->addAction(new_action);

  file_menu->addSeparator();

  auto *load = new QAction("Open", this);
  load->setShortcut(tr("Ctrl+O"));
  file_menu->addAction(load);

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

  QMenu *graph_menu = menuBar()->addMenu("&Graph");

  auto *new_graph = new QAction("New graph", this);
  graph_menu->addAction(new_graph);

  graph_menu->addSeparator();

  auto *reseed = new QAction("Reseed random generators", this);
  reseed->setShortcut(tr("Alt+R"));
  graph_menu->addAction(reseed);

  QMenu *view_menu = menuBar()->addMenu("&View");

  auto *show_layout_manager = new QAction("Graph layout manager", this);
  show_layout_manager->setCheckable(true);
  show_layout_manager->setChecked(this->graph_manager_widget->isVisible());
  view_menu->addAction(show_layout_manager);

  view_menu->addSeparator();

  auto *show_texture_downloader = new QAction("Texture downloader", this);
  show_texture_downloader->setCheckable(true);
  show_texture_downloader->setChecked(this->texture_downloader.isVisible());
  view_menu->addAction(show_texture_downloader);

  view_menu->addSeparator();

  auto *show_node_settings_pan_action = new QAction("Show node settings pan", this);
  show_node_settings_pan_action->setCheckable(true);
  show_node_settings_pan_action->setChecked(this->show_node_settings_pan);
  this->graph_tabs_widget->set_show_node_settings_widget(this->show_node_settings_pan);
  view_menu->addAction(show_node_settings_pan_action);

  QMenu *help = menuBar()->addMenu("&Help");

  auto *quick_help = new QAction("Quick help", this);
  help->addAction(quick_help);

  help->addSeparator();

  auto *about = new QAction("&About", this);
  help->addAction(about);

  // --- connections

  this->connect(new_action, &QAction::triggered, this, &MainWindow::on_new);
  this->connect(load, &QAction::triggered, this, &MainWindow::on_load);
  this->connect(quick_help, &QAction::triggered, this, &MainWindow::show_quick_help);
  this->connect(about, &QAction::triggered, this, &MainWindow::show_about);

  this->connect(show_node_settings_pan_action,
                &QAction::triggered,
                this,
                [this, show_node_settings_pan_action]()
                {
                  this->show_node_settings_pan = !this->show_node_settings_pan;
                  show_node_settings_pan_action->setChecked(this->show_node_settings_pan);
                  this->graph_tabs_widget->set_show_node_settings_widget(
                      this->show_node_settings_pan);
                });

  this->connect(show_layout_manager,
                &QAction::triggered,
                this,
                [this, show_layout_manager]()
                {
                  bool state = this->graph_manager_widget->isVisible();
                  this->graph_manager_widget->setVisible(!state);
                  show_layout_manager->setChecked(!state);
                });

  this->connect(this->graph_manager_widget.get(),
                &GraphManagerWidget::window_closed,
                [show_layout_manager]() { show_layout_manager->setChecked(false); });

  this->connect(show_texture_downloader,
                &QAction::triggered,
                this,
                [this, show_texture_downloader]()
                {
                  bool state = this->texture_downloader.isVisible();
                  this->texture_downloader.setVisible(!state);
                  show_texture_downloader->setChecked(!state);
                });

  this->connect(&this->texture_downloader,
                &qtd::TextureDownloader::window_closed,
                [show_texture_downloader]()
                { show_texture_downloader->setChecked(false); });

  // save / save as
  this->connect(save, &QAction::triggered, this, &MainWindow::on_save);
  this->connect(save_as, &QAction::triggered, this, &MainWindow::on_save_as);
  this->connect(save_copy, &QAction::triggered, this, &MainWindow::on_save_copy);
  this->connect(export_batch, &QAction::triggered, this, &MainWindow::on_export_batch);

  // --- graphs
  this->connect(new_graph,
                &QAction::triggered,
                this->graph_manager_widget.get(),
                &GraphManagerWidget::on_new_graph_request);

  this->connect(reseed,
                &QAction::triggered,
                this->graph_manager_widget.get(),
                &GraphManagerWidget::on_reseed);

  // quit
  this->connect(quit, &QAction::triggered, this, &MainWindow::on_quit);
}

void MainWindow::show_about()
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
  msg_box.exec();
}

void MainWindow::show_quick_help()
{
  Logger::log()->trace("MainWindow::show_quick_help");

  std::string html_source = "";

  std::ifstream file(HSD_QUICK_HELP_HTML);
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

void MainWindow::show_viewport()
{
  Logger::log()->trace("MainWindow::show_viewport");
  this->graph_tabs_widget->show_viewport();
}

void MainWindow::update_window_title()
{
  std::string title = "Hesiod";
  if (!this->project_path.empty())
    title += " - " + this->get_project_name();

  title += this->is_dirty ? "*" : "";

  this->set_title(title.c_str());
}

// functions

void notify(const std::string &title, const std::string &text)
{
  Toast::setPosition(ToastPosition::BOTTOM_RIGHT);

  Toast *toast = new Toast(MainWindow::instance());
  toast->setDuration(6000);
  toast->setTitle(title.c_str());
  toast->setText(text.c_str());

  // TODO hardcoded colors and parameters

  toast->setBackgroundColor(QColor("#3C3C3C"));
  toast->setTitleColor(QColor("#EFF1F2"));
  toast->setTextColor(QColor("#DFE1E2"));
  toast->setDurationBarColor(QColor("#5B5B5B"));
  toast->setIconSeparatorColor(QColor("#4772b3"));
  toast->setCloseButtonIconColor(QColor("#DFE1E2"));
  toast->setMinimumWidth(350);
  toast->setMaximumWidth(350);
  toast->setBorderRadius(8);

  toast->setShowIcon(false);
  // toast->setIconColor(QColor("#5B5B5B"));
  // toast->setIcon(QPixmap(HSD_APP_ICON));
  // toast->applyPreset(ToastPreset::ERROR); // Apply style preset

  QFont font = MainWindow::instance()->font();
  toast->setTextFont(font);
  font.setWeight(QFont::Bold);
  toast->setTitleFont(font);

  toast->show();
}

} // namespace hesiod
