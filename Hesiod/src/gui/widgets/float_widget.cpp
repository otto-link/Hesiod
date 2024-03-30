/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QHBoxLayout>
#include <QPushButton>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

FloatWidget::FloatWidget(FloatAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("Float attribute");

  QHBoxLayout *layout = new QHBoxLayout(this);

  this->slider = new QSlider(Qt::Horizontal);
  this->slider->setRange(0, HSD_SLIDER_STEPS);
  this->slider->setSingleStep(0);
  this->slider->setPageStep(0);
  int pos = (int)(HSD_SLIDER_STEPS * (this->p_attr->value - this->p_attr->vmin) /
                  (this->p_attr->vmax - this->p_attr->vmin));
  this->slider->setValue(pos);
  layout->addWidget(this->slider);

  // only display value but no signal
  connect(this->slider,
          &QSlider::valueChanged,
          [this]()
          {
            float v = slider_pos_to_float(this->slider->value(),
                                          this->p_attr->vmin,
                                          this->p_attr->vmax,
                                          HSD_SLIDER_STEPS);
            this->label->setText(QString().asprintf(this->p_attr->fmt.c_str(), v));
          });

  // signal change
  connect(this->slider, &QSlider::sliderReleased, this, &FloatWidget::update_attribute);

  this->label = new QLabel(
      QString().asprintf(this->p_attr->fmt.c_str(), this->p_attr->value));
  layout->addWidget(this->label);

  this->setLayout(layout);
}

void FloatWidget::update_attribute()
{
  this->p_attr->value = this->p_attr->vmin +
                        (float)this->slider->value() / HSD_SLIDER_STEPS *
                            (this->p_attr->vmax - this->p_attr->vmin);
  this->label->setText(
      QString().asprintf(this->p_attr->fmt.c_str(), this->p_attr->value));
  Q_EMIT this->value_changed();
}

} // namespace hesiod
