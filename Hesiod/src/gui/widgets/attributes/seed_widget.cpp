/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QHBoxLayout>
#include <QPushButton>

#include "hesiod/gui/widgets/widgets.hpp"

namespace hesiod
{

SeedWidget::SeedWidget(SeedAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("Seed attribute");

  QHBoxLayout *layout = new QHBoxLayout(this);

  this->spinbox = new QSpinBox();
  this->spinbox->setMinimum(0);
  this->spinbox->setMaximum((int)std::numeric_limits<int>::max());
  this->spinbox->setValue((int)this->p_attr->value);
  layout->addWidget(this->spinbox);

  connect(this->spinbox,
          qOverload<int>(&QSpinBox::valueChanged),
          [this]() { this->update_attribute(); });

  QPushButton *button = new QPushButton("New seed");
  layout->addWidget(button);

  // chain of signal/slot QSpinBox::setValue => QSpinBox::valueChanged
  // => SeedWidget::update_attribute
  connect(button,
          &QPushButton::released,
          [this]() { this->spinbox->setValue((uint)time(NULL)); });

  this->setLayout(layout);
}

void SeedWidget::update_attribute()
{
  this->p_attr->value = (uint)this->spinbox->value();
  Q_EMIT this->value_changed();
}

} // namespace hesiod
