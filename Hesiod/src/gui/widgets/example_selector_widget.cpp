/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/gui/widgets/example_selector_dialog.hpp"
#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/gui/widgets/table_hover_widget.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

ExampleSelectorDialog::ExampleSelectorDialog(const QString &examples_path,
                                             QWidget       *parent)
    : QDialog(parent), examples_path(examples_path)
{
  Logger::log()->trace("ExampleSelectorDialog::ExampleSelectorDialog");

  this->setWindowTitle("Select Example to Load");
  this->setModal(true);
  this->resize(700, 700);
  this->setMinimumWidth(512);
  this->setMinimumHeight(512);

  auto *main_layout = new QVBoxLayout(this);

  // title
  {
    QLabel *label = new QLabel("Hesiod - Ready-made Examples");
    label->setAlignment(Qt::AlignCenter);
    resize_font(label, +2);
    main_layout->addWidget(label);
  }

  this->table = new QTableWidget(this);
  this->table->setColumnCount(2);
  this->table->horizontalHeader()->setVisible(false);
  this->table->verticalHeader()->setVisible(false);
  this->table->setSelectionMode(QAbstractItemView::SingleSelection);
  this->table->setSelectionBehavior(QAbstractItemView::SelectItems);
  this->table->setShowGrid(false);
  this->table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  this->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  this->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  this->table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  this->table->verticalHeader()->setDefaultSectionSize(230);

  connect(this->table,
          &QTableWidget::itemDoubleClicked,
          this,
          &ExampleSelectorDialog::on_double_click);

  main_layout->addWidget(this->table);

  {
    auto *check_box = new QCheckBox("Show this window at startup");
    check_box->setChecked(
        HSD_CTX.app_settings.interface.enable_example_selector_at_startup);

    this->connect(
        check_box,
        &QCheckBox::toggled,
        this,
        [](bool value)
        { HSD_CTX.app_settings.interface.enable_example_selector_at_startup = value; });

    main_layout->addWidget(check_box);
  }

  auto *btn_layout = new QHBoxLayout();
  auto *ok_btn = new QPushButton("Open Ready-Made", this);
  auto *cancel_btn = new QPushButton("Cancel", this);
  btn_layout->addStretch();
  btn_layout->addWidget(ok_btn);
  btn_layout->addWidget(cancel_btn);
  main_layout->addLayout(btn_layout);

  connect(ok_btn, &QPushButton::clicked, this, &ExampleSelectorDialog::on_accept);
  connect(cancel_btn, &QPushButton::clicked, this, &ExampleSelectorDialog::on_reject);

  this->populate();
}

void ExampleSelectorDialog::on_accept()
{
  QList<QTableWidgetItem *> selected = this->table->selectedItems();
  if (!selected.isEmpty())
    this->selected_filename = selected.first()->data(Qt::UserRole).toString();
  else
    this->selected_filename = "";

  this->accept();
}

void ExampleSelectorDialog::on_double_click(QTableWidgetItem *item)
{
  if (!item)
    return;

  this->selected_filename = item->data(Qt::UserRole).toString();
  this->accept();
}

void ExampleSelectorDialog::on_reject()
{
  this->selected_filename = "";
  this->reject();
}

void ExampleSelectorDialog::populate()
{
  QDir        dir(this->examples_path);
  QStringList files = dir.entryList({"*.hsd"}, QDir::Files);

  const int columns = 2;
  int       rows = (files.size() + columns - 1) / columns;
  this->table->setRowCount(rows);
  this->table->setColumnCount(columns);

  int idx = 0;
  for (const QString &file : files)
  {
    int row = idx / columns;
    int col = idx % columns;
    idx++;

    QString preview_image_path = this->examples_path + "/" + file.left(file.size() - 4) +
                                 "_preview.jpg";

    QTableWidgetItem *item = new QTableWidgetItem();
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QWidget     *cell_widget = new TableHoverWidget(item);
    QVBoxLayout *cell_layout = new QVBoxLayout(cell_widget);
    cell_layout->setContentsMargins(5, 5, 5, 5);

    QLabel *name_label = new QLabel(file, cell_widget);
    name_label->setAlignment(Qt::AlignCenter);
    name_label->setMaximumHeight(24);
    cell_layout->addWidget(name_label);

    QLabel *preview = new QLabel(cell_widget);
    preview->setAlignment(Qt::AlignCenter);

    if (QFile::exists(preview_image_path))
    {
      QPixmap pix(preview_image_path);
      preview->setPixmap(
          pix.scaled(400, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
      preview->setText("No Preview");
    }

    cell_layout->addWidget(preview);
    this->table->setCellWidget(row, col, cell_widget);

    item->setData(Qt::UserRole, this->examples_path + "/" + file);
    this->table->setItem(row, col, item);
  }
}

QString ExampleSelectorDialog::selected_file() const { return this->selected_filename; }

} // namespace hesiod
