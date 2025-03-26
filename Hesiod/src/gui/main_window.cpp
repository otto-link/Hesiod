/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <iostream>

#include <QApplication>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QTabWidget>

#include "hesiod/gui/main_window.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

MainWindow::MainWindow(QApplication *p_app, QWidget *parent) : QMainWindow(parent)
{
  this->setWindowTitle(tr("Hesiod"));
  this->restore_state();

  // --- menu bar

  QMenu *file_menu = menuBar()->addMenu("&File");

  auto *load = new QAction("Open", this);
  load->setShortcut(tr("Ctrl+O"));
  file_menu->addAction(load);

  auto *save = new QAction("Save", this);
  save->setShortcut(tr("Ctrl+S"));
  file_menu->addAction(save);

  auto *save_as = new QAction("Save As", this);
  save_as->setShortcut(tr("Ctrl+Shift+S"));
  file_menu->addAction(save_as);

  auto *quit = new QAction("Quit", this);
  quit->setShortcut(tr("Ctrl+Q"));
  file_menu->addAction(quit);

  QMenu *help = menuBar()->addMenu("&Help");

  auto *about = new QAction("&About", this);
  help->addAction(about);

  // --- widgets

  QWidget    *central_widget = new QWidget(this);
  QTabWidget *tab_widget = new QTabWidget(central_widget);

  // boot graph manager
  bool headless = false;
  this->graph_manager = hesiod::GraphManager(headless);
  this->graph_manager.set_tab_widget(tab_widget);
  this->graph_manager.load_from_file("data/default.hsd");

  // {
  //   auto config = std::make_shared<hesiod::ModelConfig>();
  //   auto graph = std::make_shared<hesiod::GraphEditor>("", config);

  //   this->graph_manager.add_graph_editor(graph, "base graph");
  //   // graph->load_from_file("data/default.hsd");
  //   // graph->get_p_viewer()->zoom_to_content();
  // }

  // {
  //   auto config = std::make_shared<hesiod::ModelConfig>();
  //   auto graph = std::make_shared<hesiod::GraphEditor>("", config);

  //   this->graph_manager.add_graph_editor(graph, "another graph");
  //   // graph->load_from_file("data/default.hsd");
  //   // graph->get_p_viewer()->zoom_to_content();
  // }

  // // {
  // //   auto config = std::make_shared<hesiod::ModelConfig>();
  // //   auto graph = std::make_shared<hesiod::GraphEditor>("", config);

  // //   // graph->load_from_file("data/default.hsd");
  // //   graph->get_p_viewer()->zoom_to_content();
  // //   // this->graph_manager.add_graph_editor(graph, "third one");
  // //   this->graph_manager.add_graph_editor_after(graph, "base graph", "third one");
  // // }

  // this->graph_manager.dump();
  // this->graph_manager.update_tab_widget();

  // nlohmann::json json = this->graph_manager.json_to();
  // this->graph_manager.json_from(json);

  // this->graph_manager.save_to_file("test.hsd");
  // this->graph_manager.load_from_file("test.hsd");

  // std::cout << this->graph_manager.get_graph_ref_by_id_next("base graph")->get_id()
  //           << "\n";
  // std::cout << this->graph_manager.get_graph_ref_by_id_previous("third one")->get_id()
  //           << "\n";

  // main central widget layout
  QHBoxLayout *main_layout = new QHBoxLayout(central_widget);
  main_layout->addWidget(tab_widget);
  central_widget->setLayout(main_layout);

  this->setCentralWidget(central_widget);

  // --- connections

  connect(about, &QAction::triggered, this, &MainWindow::show_about);

  connect(
      load,
      &QAction::triggered,
      [this]()
      {
        std::filesystem::path path = this->graph_manager.get_fname_path().parent_path();

        QString load_fname = QFileDialog::getOpenFileName(this,
                                                          "Load...",
                                                          path.string().c_str(),
                                                          "Hesiod files (*.hsd)");

        if (!load_fname.isNull() && !load_fname.isEmpty())
        {
          this->graph_manager.set_fname_path(load_fname.toStdString());
          this->graph_manager.load_from_file(load_fname.toStdString());
        }
      });

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

  // save / save as
  {
    auto lambda_save_as = [this]()
    {
      std::filesystem::path path = this->graph_manager.get_fname_path().parent_path();

      QString new_fname = QFileDialog::getSaveFileName(this,
                                                       "Save as...",
                                                       path.string().c_str(),
                                                       "Hesiod files (*.hsd)");

      if (!new_fname.isNull() && !new_fname.isEmpty())
      {
        this->graph_manager.set_fname_path(new_fname.toStdString());
        this->graph_manager.save_to_file(new_fname.toStdString());
      }
    };

    connect(save,
            &QAction::triggered,
            [this, lambda_save_as]()
            {
              if (this->graph_manager.get_fname_path() == "")
                lambda_save_as();
              else
                this->graph_manager.save_to_file(this->graph_manager.get_fname_path());
            });

    connect(save_as, &QAction::triggered, lambda_save_as);
  }

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
