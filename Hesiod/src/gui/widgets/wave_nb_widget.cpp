/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QPushButton>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

WaveNbWidget::WaveNbWidget(WaveNbAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("WaveNb attribute");

  QGridLayout *layout = new QGridLayout(this);

  // --- x direction

  this->slider_x = new QSlider(Qt::Horizontal);
  this->slider_x->setRange(0, HSD_SLIDER_STEPS);
  this->slider_x->setSingleStep(0);
  this->slider_x->setPageStep(0);
  int pos = float_to_slider_pos(this->p_attr->value.x,
                                this->p_attr->vmin,
                                this->p_attr->vmax,
                                HSD_SLIDER_STEPS);
  this->slider_x->setValue(pos);
  layout->addWidget(this->slider_x, 0, 0);

  // only display value but no signal
  connect(this->slider_x,
          &QSlider::valueChanged,
          [this]()
          {
            float v = slider_pos_to_float(this->slider_x->value(),
                                          this->p_attr->vmin,
                                          this->p_attr->vmax,
                                          HSD_SLIDER_STEPS);
            this->label_x->setText(QString().asprintf(this->p_attr->fmt.c_str(), v));
          });

  // signal change
  connect(this->slider_x,
          &QSlider::sliderReleased,
          this,
          &WaveNbWidget::update_attribute);

  this->label_x = new QLabel(
      QString().asprintf(this->p_attr->fmt.c_str(), this->p_attr->value.x));
  layout->addWidget(this->label_x, 0, 1);

  // --- y direction

  this->slider_y = new QSlider(Qt::Horizontal);
  this->slider_y->setRange(0, HSD_SLIDER_STEPS);
  this->slider_y->setSingleStep(0);
  this->slider_y->setPageStep(0);
  pos = float_to_slider_pos(this->p_attr->value.y,
                            this->p_attr->vmin,
                            this->p_attr->vmax,
                            HSD_SLIDER_STEPS);
  this->slider_y->setValue(pos);
  layout->addWidget(this->slider_y, 1, 0);

  // only display value but no signal
  connect(this->slider_y,
          &QSlider::valueChanged,
          [this]()
          {
            float v = slider_pos_to_float(this->slider_y->value(),
                                          this->p_attr->vmin,
                                          this->p_attr->vmax,
                                          HSD_SLIDER_STEPS);
            this->label_y->setText(QString().asprintf(this->p_attr->fmt.c_str(), v));
          });

  // signal change
  connect(this->slider_y,
          &QSlider::sliderReleased,
          this,
          &WaveNbWidget::update_attribute);

  this->label_y = new QLabel(
      QString().asprintf(this->p_attr->fmt.c_str(), this->p_attr->value.y));
  layout->addWidget(this->label_y, 1, 1);

  // --- x/y link
  this->checkbox = new QCheckBox("link x and y");
  this->checkbox->setChecked(p_attr->link_xy);
  layout->addWidget(checkbox, 2, 0);

  connect(checkbox,
          &QCheckBox::clicked,
          [this]()
          {
            this->p_attr->link_xy = this->checkbox->checkState();
            this->update_attribute();
          });

  connect(checkbox, &QCheckBox::clicked, this->slider_y, &QSlider::setVisible);
  connect(checkbox, &QCheckBox::clicked, this->label_y, &QSlider::setVisible);

  this->setLayout(layout);
}

void WaveNbWidget::update_attribute()
{
  this->p_attr->value.x = slider_pos_to_float(this->slider_x->value(),
                                              this->p_attr->vmin,
                                              this->p_attr->vmax,
                                              HSD_SLIDER_STEPS);

  if (this->p_attr->link_xy)
    this->p_attr->value.y = this->p_attr->value.x;
  else
    this->p_attr->value.y = slider_pos_to_float(this->slider_y->value(),
                                                this->p_attr->vmin,
                                                this->p_attr->vmax,
                                                HSD_SLIDER_STEPS);

  this->label_x->setText(
      QString().asprintf(this->p_attr->fmt.c_str(), this->p_attr->value.x));
  this->label_y->setText(
      QString().asprintf(this->p_attr->fmt.c_str(), this->p_attr->value.y));

  Q_EMIT this->value_changed();
}

} // namespace hesiod
