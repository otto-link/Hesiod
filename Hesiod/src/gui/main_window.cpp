/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <QApplication>
#include <QCloseEvent>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QStatusBar>
#include <QTabWidget>

#include "macrologger.h"

#include "hesiod/gui/main_window.hpp"
#include "hesiod/gui/node_editor_widget.hpp"
#include "hesiod/gui/node_settings_widget.hpp"
#include "hesiod/model/model_config.hpp"

namespace hesiod
{

MainWindow::MainWindow(QApplication *p_app, QWidget *parent) : QMainWindow(parent)
{

  hesiod::ModelConfig model_config;
  model_config.set_shape({1024, 1024});
  model_config.tiling = {4, 4};

  this->setWindowTitle(tr("Hesiod"));
  this->create_status_bar();
  this->restore_state();

  // layout
  this->create_menu_bar();

  QWidget *central_widget = new QWidget(this);

  QHBoxLayout *layout = new QHBoxLayout(central_widget);

  QTabWidget *tab = new QTabWidget();

  auto editor = new NodeEditorWidget(model_config);
  tab->addTab(editor, "Node editor");
  layout->addWidget(tab);

  this->setCentralWidget(central_widget);

  // Create a dock widget
  QDockWidget *dock_settings = new QDockWidget("Node settings", this);
  dock_settings->setAllowedAreas(Qt::AllDockWidgetAreas);
  dock_settings->setFeatures(QDockWidget::DockWidgetMovable |
                             QDockWidget::DockWidgetFloatable);

  // Add some content to the dock widget
  NodeSettingsWidget *node_settings_widget = new NodeSettingsWidget(
      editor->get_scene_ref());

  dock_settings->setWidget(node_settings_widget);
  dock_settings->setObjectName("dock_settings");
  dock_settings->setMinimumWidth(300);

  // dock_settings->setWindowFlags(Qt::Window);

  // QLabel *title_label = new QLabel("Some title", dock_settings);
  // dock_settings->setTitleBarWidget(title_label);

  // Add the dock widget to the main window
  this->addDockWidget(Qt::RightDockWidgetArea, dock_settings);

  // --- connections

  QObject::connect(p_app, &QApplication::aboutToQuit, [&]() { this->save_state(); });
}

MainWindow::~MainWindow() { this->save_state(); }

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (true) // userReallyWantsToQuit())
  {
    this->save_state();
    event->accept();
  }
  else
    event->ignore();
}

void MainWindow::create_menu_bar()
{
  // auto *preferences = new QAction("&Preferences", this);
  auto *quit = new QAction("&Quit", this);
  quit->setShortcut(tr("CTRL+Q"));

  auto *about = new QAction("&About", this);

  QMenu *file = menuBar()->addMenu("&File");
  // file->addAction(preferences);
  // file->addSeparator();
  file->addAction(quit);

  QMenu *help = menuBar()->addMenu("&Help");
  help->addAction(about);

  connect(quit,
          &QAction::triggered,
          []()
          {
            LOG_DEBUG("quitting...");
            QApplication::quit();
          });

  connect(about, &QAction::triggered, this, &MainWindow::show_about);
}

void MainWindow::create_status_bar() { this->statusBar()->showMessage(tr("Ready")); }

void MainWindow::restore_state()
{
  QSettings settings("olink", "hesiod");
  this->restoreState(settings.value("MainWindow/state").toByteArray());
  this->restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
}

void MainWindow::set_status_bar_message(std::string msg)
{
  this->statusBar()->showMessage(QString::fromStdString(msg));
}

void MainWindow::show_about()
{
  QMessageBox msg_box;
  msg_box.setText("Hesiov vx.x.x.\nA desktop application for node-based "
                  "procedural terrain generation.\nhttps://github.com/otto-link/Hesiod");
  msg_box.exec();
}

void MainWindow::save_state()
{
  QSettings settings("olink", "hesiod");
  settings.setValue("MainWindow/state", this->saveState());
  settings.setValue("MainWindow/geometry", this->saveGeometry());

  // TODO do the same for main widgets of the main window
}

} // namespace hesiod
