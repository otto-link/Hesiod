/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QApplication>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>

#include "hesiod/gui/main_window.hpp"

namespace hesiod
{

MainWindow::MainWindow(QApplication *p_app, QWidget *parent) : QMainWindow(parent)
{
  this->setWindowTitle(tr("Hesiod"));
  this->restore_state();

  // --- menu bar

  auto *quit = new QAction("&Quit", this);
  quit->setShortcut(tr("Ctrl+Q"));

  auto *about = new QAction("&About", this);

  QMenu *file_menu = menuBar()->addMenu("&File");
  file_menu->addAction(quit);

  QMenu *help = menuBar()->addMenu("&Help");
  help->addAction(about);

  // --- widgets

  QWidget *central_widget = new QWidget(this);

  QHBoxLayout *layout = new QHBoxLayout(central_widget);

  auto config = std::make_shared<hesiod::ModelConfig>();

  {
    hesiod::GraphEditor *p_graph = new hesiod::GraphEditor("graph1", config);

    p_graph->on_new_node_request("NoiseFbm", QPointF(200, 200));
    p_graph->on_new_node_request("Remap", QPointF(400, 200));
    p_graph->on_new_node_request("GammaCorrection", QPointF(600, 200));
    p_graph->get_p_viewer()->zoom_to_content();

    this->graph_editors.push_back(p_graph);
  }

  for (auto &editor : this->graph_editors)
    layout->addWidget(editor->get_p_viewer());

  this->setCentralWidget(central_widget);

  // --- connections

  connect(about, &QAction::triggered, this, &MainWindow::show_about);

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

void MainWindow::on_quit() { QApplication::quit(); }

void MainWindow::restore_state()
{
  QSettings settings("olink", "hesiod");
  this->restoreState(settings.value("MainWindow/state").toByteArray());
  this->restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
}

void MainWindow::show_about()
{
  std::string msg = "Hesiov v";
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

void MainWindow::save_state()
{
  QSettings settings("olink", "hesiod");
  settings.setValue("MainWindow/state", this->saveState());
  settings.setValue("MainWindow/geometry", this->saveGeometry());

  // TODO do the same for main widgets of the main window
}

} // namespace hesiod
