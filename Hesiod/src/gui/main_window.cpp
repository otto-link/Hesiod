/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QTabWidget>

#include "hesiod/gui/main_window.hpp"
#include "hesiod/gui/widgets/graph_tabs_widget.hpp"
#include "hesiod/gui/widgets/graph_manager_widget.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_manager.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

MainWindow::MainWindow(QApplication *p_app, QWidget *parent) : QMainWindow(parent)
{
  this->setWindowTitle(tr("Hesiod"));
  this->restore_state();

  // model
  this->graph_manager = std::make_unique<GraphManager>();

  // GUI
  this->graph_manager_widget = std::make_unique<GraphManagerWidget>(
      this->graph_manager.get());
  this->graph_manager_widget->show();

  this->graph_tabs_widget = std::make_unique<GraphTabsWidget>(
      this->graph_manager.get());
  // this->graph_tabs_widget->show();

  this->setup_central_widget();

  // --- connections

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

  // after widgets and graph manager
  this->setup_menu_bar();

  this->connect(p_app, &QApplication::aboutToQuit, [&]() { this->save_state(); });
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

std::string MainWindow::get_project_name() const
{
  if (this->project_path.empty())
    return std::string();
  else
    return this->project_path.filename();
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

void MainWindow::load_from_file(const std::string &fname)
{
  LOG->trace("MainWindow::load_from_file: {}", fname);

  nlohmann::json json = json_from_file(fname);

  this->clear_all();

  // model
  this->graph_manager->json_from(json["graph_manager"]);
  this->graph_manager->update();

  // GUI
  this->graph_manager_widget->json_from(json["graph_manager_widget"]);
  this->graph_tabs_widget->json_from(json["graph_tabs_widget"]);
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

void MainWindow::restore_state()
{
  QSettings settings(HSD_SETTINGS_ORG, HSD_SETTINGS_APP);
  this->restoreState(settings.value("MainWindow/state").toByteArray());
  this->restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
}

void MainWindow::save_state()
{
  QSettings settings(HSD_SETTINGS_ORG, HSD_SETTINGS_APP);
  settings.setValue("MainWindow/state", this->saveState());
  settings.setValue("MainWindow/geometry", this->saveGeometry());
}

bool MainWindow::save_to_file(const std::string &fname) const
{
  LOG->trace("MainWindow::save_to_file: {}", fname);

  try
  {
    nlohmann::json json;

    // general infos
    json["Hesiod version"] = "v" + std::to_string(HESIOD_VERSION_MAJOR) + "." +
                             std::to_string(HESIOD_VERSION_MINOR) + "." +
                             std::to_string(HESIOD_VERSION_PATCH);

    // save current date and time
    auto              now = std::chrono::system_clock::now();
    std::time_t       now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream time_stream;
    time_stream << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    json["saved_at"] = time_stream.str();

    // model
    json["graph_manager"] = this->graph_manager->json_to();

    // GUI
    json["graph_manager_widget"] = this->graph_manager_widget->json_to();
    json["graph_tabs_widget"] = this->graph_tabs_widget->json_to();

    json_to_file(json, fname);
    return true;
  }
  catch (const std::exception &e)
  {
    LOG->critical("MainWindow::save_to_file: failed to save file {}, what: {}",
                  fname,
                  e.what());
    return false;
  }
}

void MainWindow::set_project_path(const std::string &new_project_path)
{
  this->set_project_path(std::filesystem::path(new_project_path));
}

void MainWindow::set_project_path(const std::filesystem::path &new_project_path)
{
  this->project_path = new_project_path;

  std::string title = "Hesiod";
  if (!this->project_path.empty())
    title += " - " + this->get_project_name();

  this->set_title(title.c_str());
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

  file_menu->addSeparator();

  auto *quit = new QAction("Quit", this);
  quit->setShortcut(tr("Ctrl+Q"));
  file_menu->addAction(quit);

  QMenu *graph_menu = menuBar()->addMenu("&Graph");

  auto *new_graph = new QAction("New graph", this);
  graph_menu->addAction(new_graph);

  QMenu *view_menu = menuBar()->addMenu("&View");

  auto *show_layout_manager = new QAction("Graph layout manager", this);
  show_layout_manager->setCheckable(true);
  show_layout_manager->setChecked(true);
  view_menu->addAction(show_layout_manager);

  QMenu *help = menuBar()->addMenu("&Help");

  auto *about = new QAction("&About", this);
  help->addAction(about);

  // --- connections

  this->connect(new_action, &QAction::triggered, this, &MainWindow::on_new);
  this->connect(load, &QAction::triggered, this, &MainWindow::on_load);
  this->connect(about, &QAction::triggered, this, &MainWindow::show_about);

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

  // save / save as
  {
    auto lambda_save_as = [this]()
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
      }
    };

    this->connect(save,
                  &QAction::triggered,
                  [this, lambda_save_as]()
                  {
                    if (this->project_path.empty())
                      lambda_save_as();
                    else
                      this->save_to_file(this->project_path.string());
                  });

    this->connect(save_as, &QAction::triggered, lambda_save_as);
  }

  // --- graphs
  this->connect(new_graph,
                &QAction::triggered,
                this->graph_manager_widget.get(),
                &GraphManagerWidget::on_new_graph_request);

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

} // namespace hesiod
