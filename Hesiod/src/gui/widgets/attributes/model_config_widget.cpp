/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QDialogButtonBox>
#include <QGridLayout>

#include "hesiod/gui/widgets/widgets.hpp"

namespace hesiod
{

ModelConfigWidget::ModelConfigWidget(ModelConfig *p_model_config, QWidget *parent)
    : QDialog(parent), p_model_config(p_model_config)
{
  QGridLayout *layout = new QGridLayout(this);

  int row = 0;

  // shape

  QLabel *label_shape_text = new QLabel("shape");
  layout->addWidget(label_shape_text, row, 0);

  this->slider_shape = new QSlider(Qt::Horizontal);
  this->slider_shape->setRange(8, 12); // 256 -> 4096
  this->slider_shape->setSingleStep(1);
  this->slider_shape->setPageStep(1);
  int pos = (int)std::log2(this->p_model_config->shape.x);
  this->slider_shape->setValue(pos);
  layout->addWidget(this->slider_shape, row, 1);

  this->label_shape = new QLabel(QString().asprintf("%dx%d",
                                                    this->p_model_config->shape.x,
                                                    this->p_model_config->shape.y));
  layout->addWidget(this->label_shape, row, 2);

  connect(this->slider_shape,
          &QSlider::valueChanged,
          [this]()
          {
            int pos = this->slider_shape->value();
            this->p_model_config->set_shape(
                hmap::Vec2<int>(std::pow(2, pos), std::pow(2, pos)));
            this->label_shape->setText(QString().asprintf("%dx%d",
                                                          this->p_model_config->shape.x,
                                                          this->p_model_config->shape.y));
          });

  row++;

  // tiling

  QLabel *label_tiling_text = new QLabel("tiling");
  layout->addWidget(label_tiling_text, row, 0);

  this->slider_tiling = new QSlider(Qt::Horizontal);
  this->slider_tiling->setRange(1, 8);
  this->slider_tiling->setSingleStep(1);
  this->slider_tiling->setPageStep(1);
  this->slider_tiling->setValue(this->p_model_config->tiling.x);
  layout->addWidget(this->slider_tiling, row, 1);

  this->label_tiling = new QLabel(QString().asprintf("%dx%d",
                                                     this->p_model_config->tiling.x,
                                                     this->p_model_config->tiling.y));
  layout->addWidget(this->label_tiling, row, 2);

  connect(this->slider_tiling,
          &QSlider::valueChanged,
          [this]()
          {
            int pos = this->slider_tiling->value();
            this->p_model_config->tiling.x = pos;
            this->p_model_config->tiling.y = pos;
            this->label_tiling->setText(
                QString().asprintf("%dx%d",
                                   this->p_model_config->tiling.x,
                                   this->p_model_config->tiling.y));
          });

  row++;

  // overlap

  QLabel *label_overlap_text = new QLabel("overlap");
  layout->addWidget(label_overlap_text, row, 0);

  this->slider_overlap = new QSlider(Qt::Horizontal);
  this->slider_overlap->setRange(0, this->steps);
  this->slider_overlap->setSingleStep(1);
  this->slider_overlap->setPageStep(1);
  pos = float_to_slider_pos(this->p_model_config->overlap,
                            this->vmin,
                            this->vmax,
                            this->steps);
  this->slider_overlap->setValue(pos);
  layout->addWidget(this->slider_overlap, row, 1);

  this->label_overlap = new QLabel(
      QString().asprintf("%.2f", this->p_model_config->overlap));
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
            this->p_model_config->overlap = v;
          });

  row++;

  // buttons

  QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                      QDialogButtonBox::Cancel);

  layout->addWidget(button_box, row, 0, 1, 3);

  this->setLayout(layout);

  connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

} // namespace hesiod
