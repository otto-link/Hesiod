/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QMainWindow>

#include "hesiod/gui/node_editor_widget.hpp"
#include "hesiod/gui/viewer2d_widget.hpp"
#include "hesiod/gui/viewer3d_widget.hpp"
#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QApplication *p_app = nullptr, QWidget *p_parent = nullptr);

  ~MainWindow() override;

  hesiod::Viewer2dWidget *viewer2d;

  hesiod::Viewer3dWidget *viewer3d;

protected:
  void restore_state();

  void save_state();

  void set_status_bar_message(std::string msg);

  void show_about();

private:
  void closeEvent(QCloseEvent *event) override;

  hesiod::ModelConfig model_config;

  NodeEditorWidget *node_editor_widget;
};

} // namespace hesiod
