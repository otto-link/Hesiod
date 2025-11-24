/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>

#include "hesiod/gui/widgets/flatten_config_dialog.hpp"
#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

FlattenConfigDialog::FlattenConfigDialog(FlattenConfig &export_param, QWidget *parent)
    : QDialog(parent), export_param(export_param)
{
  Logger::log()->trace("FlattenConfigDialog::FlattenConfigDialog");

  this->setWindowTitle("Hesiod - Graph flatten configuration");

  QGridLayout *layout = new QGridLayout(this);

  int row = 0;

  // --- filename

  QLabel *label_fname = new QLabel("filename");
  layout->addWidget(label_fname, row, 0);

  QPushButton *button = new QPushButton(this->export_param.export_path.string().c_str());
  layout->addWidget(button, row, 1, 1, 2);
  row++;

  this->connect(
      button,
      &QPushButton::released,
      [this]()
      {
        std::filesystem::path path = this->export_param.export_path.parent_path();

        QString fname;

        fname = QFileDialog::getSaveFileName(this,
                                             "Select Export File",
                                             path.string().c_str(),
                                             "*.png");

        if (!fname.isNull() && !fname.isEmpty())
          this->export_param.export_path = fname.toStdString();
      });

  // --- shape

  QLabel *label_shape_text = new QLabel("shape");
  layout->addWidget(label_shape_text, row, 0);

  this->slider_shape = new QSlider(Qt::Horizontal);
  this->slider_shape->setRange(8, 12); // 256 -> 4096
  this->slider_shape->setSingleStep(1);
  this->slider_shape->setPageStep(1);
  int pos = (int)std::log2(this->export_param.shape.x);
  this->slider_shape->setValue(pos);
  layout->addWidget(this->slider_shape, row, 1);

  this->label_shape = new QLabel(QString().asprintf("%dx%d",
                                                    this->export_param.shape.x,
                                                    this->export_param.shape.y));
  layout->addWidget(this->label_shape, row, 2);

  connect(this->slider_shape,
          &QSlider::valueChanged,
          [this]()
          {
            int pos = this->slider_shape->value();
            this->export_param.shape = hmap::Vec2<int>(std::pow(2, pos),
                                                       std::pow(2, pos));
            this->label_shape->setText(QString().asprintf("%dx%d",
                                                          this->export_param.shape.x,
                                                          this->export_param.shape.y));
          });

  row++;

  // --- tiling

  QLabel *label_tiling_text = new QLabel("tiling");
  layout->addWidget(label_tiling_text, row, 0);

  this->slider_tiling = new QSlider(Qt::Horizontal);
  this->slider_tiling->setRange(1, 8);
  this->slider_tiling->setSingleStep(1);
  this->slider_tiling->setPageStep(1);
  this->slider_tiling->setValue(this->export_param.tiling.x);
  layout->addWidget(this->slider_tiling, row, 1);

  this->label_tiling = new QLabel(QString().asprintf("%dx%d",
                                                     this->export_param.tiling.x,
                                                     this->export_param.tiling.y));
  layout->addWidget(this->label_tiling, row, 2);

  connect(this->slider_tiling,
          &QSlider::valueChanged,
          [this]()
          {
            int pos = this->slider_tiling->value();
            this->export_param.tiling.x = pos;
            this->export_param.tiling.y = pos;
            this->label_tiling->setText(QString().asprintf("%dx%d",
                                                           this->export_param.tiling.x,
                                                           this->export_param.tiling.y));
          });

  row++;

  // --- overlap

  QLabel *label_overlap_text = new QLabel("overlap");
  layout->addWidget(label_overlap_text, row, 0);

  this->slider_overlap = new QSlider(Qt::Horizontal);
  this->slider_overlap->setRange(0, this->steps);
  this->slider_overlap->setSingleStep(1);
  this->slider_overlap->setPageStep(1);
  pos = float_to_slider_pos(this->export_param.overlap,
                            this->vmin,
                            this->vmax,
                            this->steps);
  this->slider_overlap->setValue(pos);
  layout->addWidget(this->slider_overlap, row, 1);

  this->label_overlap = new QLabel(
      QString().asprintf("%.2f", this->export_param.overlap));
  layout->addWidget(this->label_overlap, row, 2);

  connect(this->slider_overlap,
          &QSlider::valueChanged,
          [this]()
          {
            float v = slider_pos_to_float(this->slider_overlap->value(),
                                          this->vmin,
                                          this->vmax,
                                          this->steps);
            this->label_overlap->setText(QString().asprintf("%.2f", v));
            this->export_param.overlap = v;
          });

  row++;

  // --- buttons

  QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                      QDialogButtonBox::Cancel);

  layout->addWidget(button_box, row, 0, 1, 3);

  this->setLayout(layout);

  connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

} // namespace hesiod
