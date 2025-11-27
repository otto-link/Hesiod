/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QDialog>
#include <QDir>
#include <QHeaderView>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace hesiod
{

class ExampleSelectorDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ExampleSelectorDialog(const QString &examples_path, QWidget *parent = nullptr);

  QString selected_file() const;

private:
  void on_accept();
  void on_double_click(QTableWidgetItem *item);
  void on_reject();
  void populate();

  QString       examples_path;
  QTableWidget *table = nullptr;
  QString       selected_filename;
};

} // namespace hesiod