/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QHBoxLayout>

#include "hesiod/gui/widgets/widgets.hpp"

namespace hesiod
{

BoolWidget::BoolWidget(BoolAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("Bool attribute");

  QHBoxLayout *layout = new QHBoxLayout(this);

  this->checkbox = new QCheckBox(this->p_attr->get_label().c_str());
  this->checkbox->setChecked(this->p_attr->value);
  layout->addWidget(checkbox);

  connect(this->checkbox, &QCheckBox::clicked, [this]() { this->update_attribute(); });

  this->setLayout(layout);
}

void BoolWidget::update_attribute()
{
  this->p_attr->value = this->checkbox->checkState();
  Q_EMIT this->value_changed();
}

} // namespace hesiod
