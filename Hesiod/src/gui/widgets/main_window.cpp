/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QStatusBar>

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/gui/widgets/main_window.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph/graph_manager.hpp"

namespace hesiod
{

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  Logger::log()->trace("MainWindow::MainWindow");

  this->restore_geometry();
  this->setup_progress_bar();
}

void MainWindow::notify(const std::string &msg, int timeout)
{
  this->statusBar()->showMessage(msg.c_str(), timeout);
}

void MainWindow::restore_geometry()
{
  Logger::log()->trace("MainWindow::restore_geometry");

  AppContext &ctx = HSD_CTX;

  this->setGeometry(ctx.app_settings.window.x,
                    ctx.app_settings.window.y,
                    ctx.app_settings.window.w,
                    ctx.app_settings.window.h);
}

void MainWindow::save_geometry() const
{
  Logger::log()->trace("MainWindow::save_geometry");

  AppContext &ctx = HSD_CTX;

  QRect geom = this->geometry();
  ctx.app_settings.window.x = geom.x();
  ctx.app_settings.window.y = geom.y();
  ctx.app_settings.window.w = geom.width();
  ctx.app_settings.window.h = geom.height();
}

void MainWindow::setup_connections_with_project()
{
  Logger::log()->trace("MainWindow::setup_connections_with_project");

  AppContext &ctx = HSD_CTX;

  // make sure project is ready
  if (!ctx.project_model->get_graph_manager_ref())
  {
    Logger::log()->error("MainWindow::setup_connections_with_project: graph_manager "
                         "model ref is dangling ptr");
    return;
  }

  // GraphNode model -> MainWindow
  this->connect(ctx.project_model->get_graph_manager_ref(),
                &GraphManager::update_progress,
                this->progress_bar,
                [this](float progress)
                {
                  if (progress == 0.f || progress == 100.f)
                  {
                    this->progress_bar->setValue(0);
                    this->progress_bar->setTextVisible(false);

                    const std::string message = (progress == 0.f)
                                                    ? "Updating graph..."
                                                    : "Graph updated successfully.";

                    this->notify(message);
                    return;
                  }

                  this->progress_bar->setTextVisible(true);
                  this->progress_bar->setValue(static_cast<int>(progress));
                });
}

void MainWindow::setup_progress_bar()
{
  Logger::log()->trace("MainWindow::setup_progress_bar");

  AppContext &ctx = HSD_CTX;

  this->progress_bar = new QProgressBar(this);
  this->progress_bar->setRange(0, 100);
  this->progress_bar->setValue(0);
  this->progress_bar->setTextVisible(false);
  this->progress_bar->setFixedWidth(ctx.app_settings.window.progress_bar_width);

  const std::string sheet = std::format(
      R"(
        QProgressBar {{
            border: 0px;
            border-radius: 0px;
            background-color: {};
            height: 8px;
            padding: 0px;
            font-size: 10px;
        }}
        QProgressBar::chunk {{
            background-color: {};
            border-radius: 0px;
            margin: 0px;
        }}
    )",
      ctx.app_settings.colors.bg_primary.name().toStdString(),
      ctx.app_settings.colors.bg_secondary.name().toStdString());

  this->progress_bar->setStyleSheet(sheet.c_str());

  this->statusBar()->addPermanentWidget(this->progress_bar, 0);
}

} // namespace hesiod
