/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QDialog>
#include <QVector>

class QCheckBox;
class QGridLayout;
class QKeyEvent;
class QLabel;
class QLineEdit;
class QPushButton;
class QResizeEvent;
class QScrollArea;
class QStackedWidget;
class QVBoxLayout;
class QWidget;

namespace hesiod
{

class ProjectCard;

class ExampleSelectorDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ExampleSelectorDialog(const QString &examples_path, QWidget *parent = nullptr);

  QString selected_file() const;
  bool    selected_is_project() const;

protected:
  void paintEvent(QPaintEvent *event) override;
  void changeEvent(QEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private:
  void apply_theme();
  void accept_path(const QString &path, bool project);
  void accept_card(ProjectCard *card);
  void clear_cards();
  void on_accept();
  void on_reject();
  void open_project();
  void rebuild_cards();
  void rebuild_recent_projects();
  void relayout_cards();
  void select_card(ProjectCard *card);
  void show_examples();
  void show_welcome();

  QString                examples_path;
  QString                selected_filename;
  bool                   selected_project = false;
  QStackedWidget        *pages = nullptr;
  QWidget               *welcome_page = nullptr;
  QWidget               *examples_page = nullptr;
  QWidget               *recent_host = nullptr;
  QVBoxLayout           *recent_layout = nullptr;
  QLineEdit             *recent_search = nullptr;
  QLineEdit             *search = nullptr;
  QLabel                *result_count = nullptr;
  QLabel                *empty_state = nullptr;
  QScrollArea           *scroll_area = nullptr;
  QWidget               *cards_host = nullptr;
  QGridLayout           *cards_layout = nullptr;
  QPushButton           *open_button = nullptr;
  QPushButton           *secondary_button = nullptr;
  QCheckBox             *startup_check = nullptr;
  QVector<ProjectCard *> cards;
  ProjectCard           *selected_card = nullptr;
  int                    card_columns = 0;
};

} // namespace hesiod
