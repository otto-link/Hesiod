/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QHBoxLayout>
#include <QPushButton>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

IntWidget::IntWidget(IntAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("Int attribute");

  QHBoxLayout *layout = new QHBoxLayout(this);

  this->slider = new QSlider(Qt::Horizontal);
  this->slider->setRange(this->p_attr->vmin, this->p_attr->vmax);
  this->slider->setSingleStep(0);
  this->slider->setPageStep(0);
  this->slider->setValue(this->p_attr->value);
  layout->addWidget(this->slider);

  // only display value but no signal
  connect(this->slider,
          &QSlider::valueChanged,
          [this]() { this->label->setText(QString::number(this->slider->value())); });

  // signal change
  connect(this->slider, &QSlider::sliderReleased, this, &IntWidget::update_attribute);

  this->label = new QLabel(QString::number(this->p_attr->value));
  layout->addWidget(this->label);

  this->setLayout(layout);
}

void IntWidget::update_attribute()
{
  this->p_attr->value = this->slider->value();
  this->label->setText(QString::number(this->p_attr->value));
  Q_EMIT this->value_changed();
}

} // namespace hesiod
