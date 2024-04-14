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
  // (config instance saved by the node editor)
  this->model_config.set_shape({1024, 1024});
  this->model_config.tiling = {4, 4};
  this->model_config.overlap = 0.25f;

  this->setWindowTitle(tr("Hesiod"));
  this->restore_state();

  // --- menu bar
  auto *new_action = new QAction("&New", this);
  new_action->setShortcut(tr("CTRL+N"));

  auto *load_action = new QAction("&Load", this);
  load_action->setShortcut(tr("CTRL+O"));

  auto *save_action = new QAction("&Save", this);
  save_action->setShortcut(tr("CTRL+S"));

  auto *quit = new QAction("&Quit", this);
  quit->setShortcut(tr("CTRL+Q"));

  auto *about = new QAction("&About", this);

  QMenu *file = menuBar()->addMenu("&File");
  file->addAction(new_action);
  file->addSeparator();
  file->addAction(load_action);
  file->addAction(save_action);
  file->addSeparator();
  file->addAction(quit);

  QMenu *view_menu = menuBar()->addMenu("View");

  QAction *node_settings_action = view_menu->addAction("Node settings");
  node_settings_action->setCheckable(true);
  node_settings_action->setChecked(true);
  view_menu->addAction(node_settings_action);

  QAction *view2d_action = view_menu->addAction("Add 2D view");
  view2d_action->setCheckable(true);
  view2d_action->setChecked(false);
  view_menu->addAction(view2d_action);

  QAction *view3d_action = view_menu->addAction("Add 3D view");
  view3d_action->setCheckable(true);
  view3d_action->setChecked(true);
  view_menu->addAction(view3d_action);

  QMenu *help = menuBar()->addMenu("&Help");
  help->addAction(about);

  // --- widgets

  QWidget *central_widget = new QWidget(this);

  QHBoxLayout *layout = new QHBoxLayout(central_widget);

  this->node_editor_widget = new NodeEditorWidget(&this->model_config);
  layout->addWidget(this->node_editor_widget);

  this->setCentralWidget(central_widget);

  // node settings dock
  QDockWidget *dock_settings = new QDockWidget("Node settings", this);
  dock_settings->setAllowedAreas(Qt::AllDockWidgetAreas);
  dock_settings->setFeatures(QDockWidget::DockWidgetMovable |
                             QDockWidget::DockWidgetFloatable);

  NodeSettingsWidget *node_settings_widget = new NodeSettingsWidget(
      this->node_editor_widget->get_scene_ref());

  dock_settings->setWidget(node_settings_widget);
  dock_settings->setObjectName("dock_settings");
  dock_settings->setMinimumWidth(300);

  // viewer 2D dock
  QDockWidget *dock_viewer2d = new QDockWidget("Viewer 2D", this);
  dock_viewer2d->setAllowedAreas(Qt::AllDockWidgetAreas);
  dock_viewer2d->setFeatures(QDockWidget::DockWidgetMovable |
                             QDockWidget::DockWidgetFloatable);

  this->viewer2d = new hesiod::Viewer2dWidget(&this->model_config,
                                              this->node_editor_widget->get_scene_ref());

  dock_viewer2d->setWidget(this->viewer2d);
  dock_viewer2d->setObjectName("dock_viewer2d");
  dock_viewer2d->setVisible(false);

  // TODO solve OpenGL issues when docking QOpenGLWidget...

  // // viewer 3D dock
  // QDockWidget *dock_viewer3d = new QDockWidget("Node settings", this, Qt::Window);
  // dock_viewer3d->setAllowedAreas(Qt::AllDockWidgetAreas);
  // dock_viewer3d->setFeatures(QDockWidget::DockWidgetMovable |
  //                            QDockWidget::DockWidgetFloatable);

  // this->viewer3d = new
  // hesiod::Viewer3dWidget(&this->model_config,
  //                                             this->node_editor_widget->get_scene_ref());

  // dock_viewer3d->setWidget(this->viewer3d);
  // dock_viewer3d->setObjectName("dock_viewer3d");

  // Add the dock widget to the main window
  this->addDockWidget(Qt::RightDockWidgetArea, dock_viewer2d);
  this->addDockWidget(Qt::RightDockWidgetArea, dock_settings);

  // this->addDockWidget(Qt::TopDockWidgetArea, dock_viewer3d);

  this->viewer3d = new hesiod::Viewer3dWidget(&this->model_config,
                                              this->node_editor_widget->get_scene_ref());
  this->viewer3d->show();

  // --- connections

  connect(dock_settings,
          &QDockWidget::topLevelChanged,
          [dock_settings]()
          {
            if (dock_settings->isVisible() && dock_settings->isFloating())
            {
              dock_settings->setWindowFlags(Qt::Window);
              dock_settings->show();
            }
          });

  connect(dock_viewer2d,
          &QDockWidget::topLevelChanged,
          [dock_viewer2d]()
          {
            if (dock_viewer2d->isVisible() && dock_viewer2d->isFloating())
            {
              dock_viewer2d->setWindowFlags(Qt::Window);
              dock_viewer2d->show();
            }
          });

  // connect(dock_viewer3d,
  //         &QDockWidget::topLevelChanged,
  //         [dock_viewer3d]()
  //         {
  //           if (dock_viewer3d->isVisible() && dock_viewer3d->isFloating())
  //           {
  //             dock_viewer3d->setWindowFlags(Qt::Window);
  //             dock_viewer3d->show();
  //           }
  //         });

  connect(new_action,
          &QAction::triggered,
          [this]()
          {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this,
                                          "New graph",
                                          "Erase everything and start a new graph?",
                                          QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes)
              this->node_editor_widget->get_scene_ref()->clearScene();
          });

  connect(load_action,
          &QAction::triggered,
          [this]()
          {
            // reset current views
            this->viewer2d->reset();
            this->viewer3d->reset();

            // load graph
            this->node_editor_widget->load();

            // re-set various buffers based on the the model
            // configuration (e.g.  array shape)
            this->viewer2d->reset();
            this->viewer3d->reset();
          });

  connect(save_action,
          &QAction::triggered,
          this->node_editor_widget,
          &NodeEditorWidget::save);

  connect(node_settings_action,
          &QAction::toggled,
          [this, dock_settings, node_settings_action]()
          {
            if (node_settings_action->isChecked())
              dock_settings->setVisible(true);
            else
              dock_settings->setVisible(false);
          });

  connect(view2d_action,
          &QAction::toggled,
          [this, dock_viewer2d, view2d_action]()
          {
            if (view2d_action->isChecked())
              dock_viewer2d->setVisible(true);
            else
              dock_viewer2d->setVisible(false);
          });

  connect(view3d_action,
          &QAction::toggled,
          [this, view3d_action]()
          {
            if (view3d_action->isChecked())
            {
              this->viewer3d->show();
              this->viewer3d->update_viewport();
            }
            else
              this->viewer3d->hide();
          });

  connect(quit,
          &QAction::triggered,
          []()
          {
            LOG_DEBUG("quitting...");
            QApplication::quit();
          });

  connect(about, &QAction::triggered, this, &MainWindow::show_about);

  connect(p_app, &QApplication::aboutToQuit, [&]() { this->save_state(); });
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
                  "procedural terrain "
                  "generation.\nhttps://github.com/otto-link/Hesiod");
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
