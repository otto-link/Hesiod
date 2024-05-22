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
  this->setWindowTitle(tr("Hesiod"));
  this->restore_state();

  // --- menu bar

  auto *new_action = new QAction("&New", this);
  new_action->setShortcut(tr("Ctrl+N"));

  auto *load_action = new QAction("&Load", this);
  load_action->setShortcut(tr("Ctrl+O"));

  auto *save_action = new QAction("&Save", this);
  save_action->setShortcut(tr("Ctrl+S"));

  auto *quit = new QAction("&Quit", this);
  quit->setShortcut(tr("Ctrl+Q"));

  auto *about = new QAction("&About", this);

  QMenu *file_menu = menuBar()->addMenu("&File");
  file_menu->addAction(new_action);
  file_menu->addSeparator();
  file_menu->addAction(load_action);
  file_menu->addAction(save_action);
  file_menu->addSeparator();
  file_menu->addAction(quit);

  QMenu *help = menuBar()->addMenu("&Help");
  help->addAction(about);

  // --- widgets

  QWidget *central_widget = new QWidget(this);

  QHBoxLayout *layout = new QHBoxLayout(central_widget);

  this->node_editor_widgets.push_back(new NodeEditorWidget("graph_1"));

  for (auto &e : this->node_editor_widgets)
    layout->addWidget(e);

  this->setCentralWidget(central_widget);

  // --- connections

  connect(about, &QAction::triggered, this, &MainWindow::show_about);
  connect(load_action, &QAction::triggered, this, &MainWindow::on_load);
  connect(new_action, &QAction::triggered, this, &MainWindow::on_new);
  connect(save_action, &QAction::triggered, this, &MainWindow::on_save);

  connect(quit,
          &QAction::triggered,
          [this]()
          {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(nullptr,
                                          "Quit",
                                          "Quitting the application, are you sure?",
                                          QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes)
              this->on_quit();
          });

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

void MainWindow::on_load()
{
  QString filename = QFileDialog::getOpenFileName(nullptr,
                                                  tr("Hesiod"),
                                                  QDir::homePath(),
                                                  tr("Hesiod Files (*.hsd)"));

  if (!QFileInfo::exists(filename))
    return;

  for (auto &e : this->node_editor_widgets)
    e->load(filename.toStdString());
}

void MainWindow::on_new()
{
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(this,
                                "New graph",
                                "Erase everything and start a new graph?",
                                QMessageBox::Yes | QMessageBox::No);
  if (reply == QMessageBox::Yes)
    for (auto &e : this->node_editor_widgets)
      e->clear();
}

void MainWindow::on_quit() { QApplication::quit(); }

void MainWindow::on_save()
{
  QString filename = QFileDialog::getSaveFileName(nullptr,
                                                  tr("Hesiod"),
                                                  QDir::homePath(),
                                                  tr("Hesiod Files (*.hsd)"));

  if (!filename.isEmpty())
  {
    if (!filename.endsWith("hsd", Qt::CaseInsensitive))
      filename += ".hsd";
  }

  for (auto &e : this->node_editor_widgets)
    e->save(filename.toStdString());
}

void MainWindow::restore_state()
{
  QSettings settings("olink", "hesiod");
  this->restoreState(settings.value("MainWindow/state").toByteArray());
  this->restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
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
