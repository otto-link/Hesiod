/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QPushButton>

#include "hesiod/gui/widgets/widgets.hpp"

namespace hesiod
{

RangeWidget::RangeWidget(RangeAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("Range attribute");

  QGridLayout *layout = new QGridLayout(this);

  // min
  this->slider_min = new QSlider(Qt::Horizontal);
  this->slider_min->setRange(0, HSD_SLIDER_STEPS);
  this->slider_min->setSingleStep(0);
  this->slider_min->setPageStep(0);
  int pos = float_to_slider_pos(this->p_attr->value.x,
                                this->p_attr->vmin,
                                this->p_attr->vmax,
                                HSD_SLIDER_STEPS);
  this->slider_min->setValue(pos);
  layout->addWidget(this->slider_min, 0, 0);

  // only display value but no signal
  connect(this->slider_min,
          &QSlider::valueChanged,
          [this]()
          {
            float v = slider_pos_to_float(this->slider_min->value(),
                                          this->p_attr->vmin,
                                          this->p_attr->vmax,
                                          HSD_SLIDER_STEPS);
            this->label_min->setText(QString().asprintf(this->p_attr->fmt.c_str(), v));
          });

  // signal change
  connect(this->slider_min,
          &QSlider::sliderReleased,
          this,
          &RangeWidget::update_attribute);

  this->label_min = new QLabel(
      QString().asprintf(this->p_attr->fmt.c_str(), this->p_attr->value.x));
  layout->addWidget(this->label_min, 0, 1);

  // max
  this->slider_max = new QSlider(Qt::Horizontal);
  this->slider_max->setRange(0, HSD_SLIDER_STEPS);
  this->slider_max->setSingleStep(0);
  this->slider_max->setPageStep(0);
  pos = float_to_slider_pos(this->p_attr->value.y,
                            this->p_attr->vmin,
                            this->p_attr->vmax,
                            HSD_SLIDER_STEPS);
  this->slider_max->setValue(pos);
  layout->addWidget(this->slider_max, 1, 0);

  // only display value but no signal
  connect(this->slider_max,
          &QSlider::valueChanged,
          [this]()
          {
            float v = slider_pos_to_float(this->slider_max->value(),
                                          this->p_attr->vmin,
                                          this->p_attr->vmax,
                                          HSD_SLIDER_STEPS);
            this->label_max->setText(QString().asprintf(this->p_attr->fmt.c_str(), v));
          });

  // signal change
  connect(this->slider_max,
          &QSlider::sliderReleased,
          this,
          &RangeWidget::update_attribute);

  this->label_max = new QLabel(
      QString().asprintf(this->p_attr->fmt.c_str(), this->p_attr->value.y));
  layout->addWidget(this->label_max, 1, 1);

  // buttons
  QPushButton *button_center = new QPushButton("Center");
  layout->addWidget(button_center, 2, 0);

  connect(button_center,
          &QPushButton::released,
          [this]()
          {
            this->p_attr->vmax = std::max(std::abs(this->p_attr->vmin),
                                          std::abs(this->p_attr->vmax));
            this->p_attr->vmin = -this->p_attr->vmax;
            float vptp = this->p_attr->value.y - this->p_attr->value.x;
            float v0 = float_to_slider_pos(-0.5f * vptp,
                                           this->p_attr->vmin,
                                           this->p_attr->vmax,
                                           HSD_SLIDER_STEPS);
            float v1 = float_to_slider_pos(0.5f * vptp,
                                           this->p_attr->vmin,
                                           this->p_attr->vmax,
                                           HSD_SLIDER_STEPS);

            this->slider_min->setValue(v0);
            this->slider_max->setValue(v1);
            this->update_attribute();
          });

  QPushButton *button_reset = new QPushButton("Reset");
  layout->addWidget(button_reset, 2, 1);

  connect(button_reset,
          &QPushButton::released,
          [this]()
          {
            float v0 = float_to_slider_pos(0.f,
                                           this->p_attr->vmin,
                                           this->p_attr->vmax,
                                           HSD_SLIDER_STEPS);
            float v1 = float_to_slider_pos(1.f,
                                           this->p_attr->vmin,
                                           this->p_attr->vmax,
                                           HSD_SLIDER_STEPS);

            this->slider_min->setValue(v0);
            this->slider_max->setValue(v1);
            this->update_attribute();
          });

  this->setLayout(layout);
}

void RangeWidget::update_attribute()
{
  this->p_attr->value.x = slider_pos_to_float(this->slider_min->value(),
                                              this->p_attr->vmin,
                                              this->p_attr->vmax,
                                              HSD_SLIDER_STEPS);
  this->p_attr->value.y = slider_pos_to_float(this->slider_max->value(),
                                              this->p_attr->vmin,
                                              this->p_attr->vmax,
                                              HSD_SLIDER_STEPS);

  // prevent a "reverse" range
  if (this->p_attr->value.y < this->p_attr->value.x)
  {
    this->p_attr->value.y = this->p_attr->value.x;
    int pos = float_to_slider_pos(this->p_attr->value.y,
                                  this->p_attr->vmin,
                                  this->p_attr->vmax,
                                  HSD_SLIDER_STEPS);
    this->slider_max->setValue(pos);
  }

  this->label_min->setText(
      QString().asprintf(this->p_attr->fmt.c_str(), this->p_attr->value.x));
  this->label_max->setText(
      QString().asprintf(this->p_attr->fmt.c_str(), this->p_attr->value.y));

  Q_EMIT this->value_changed();
}

} // namespace hesiod
