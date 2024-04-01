/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QMainWindow>

namespace hesiod
{

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QApplication *p_app = nullptr, QWidget *p_parent = nullptr);

  ~MainWindow() override;

protected:
  void create_menu_bar();

  void create_status_bar();

  void restore_state();

  void set_status_bar_message(std::string msg);

  void show_about();

  void save_state();

private:
  void closeEvent(QCloseEvent *event) override;
};

} // namespace hesiod
