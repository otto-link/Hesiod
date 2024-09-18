/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QSlider>

#include "hesiod/gui/widgets/widgets.hpp"

namespace hesiod
{

VecIntWidget::VecIntWidget(VecIntAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("VecInt attribute");

  QGridLayout *layout = new QGridLayout(this);

  int row = 0;
  int col = 0;

  for (auto &v : this->p_attr->value)
  {
    this->labels.push_back(new QLabel());
    this->labels.back()->setAlignment(Qt::AlignCenter);
    this->labels.back()->setText(QString::number(v));
    layout->addWidget(this->labels.back(), row, col++, Qt::AlignHCenter);

    this->sliders.push_back(new QSlider(Qt::Vertical));
    this->sliders.back()->setRange(this->p_attr->vmin, this->p_attr->vmax);
    this->sliders.back()->setSingleStep(0);
    this->sliders.back()->setPageStep(0);
    this->sliders.back()->setValue(v);
    layout->addWidget(this->sliders.back(), row + 1, col++);
    col++;
  }

  for (size_t k = 0; k < this->sliders.size(); k++)
  {
    // only display value but no signal
    connect(this->sliders[k],
            &QSlider::valueChanged,
            [this, k]()
            { this->labels[k]->setText(QString::number(this->sliders[k]->value())); });

    // signal change
    connect(this->sliders[k],
            &QSlider::sliderReleased,
            this,
            &VecIntWidget::update_attribute);
  }

  // row += 2;
  // TODO add vector size change (plus / minus buttons)

  this->setLayout(layout);
}

void VecIntWidget::update_attribute()
{
  for (size_t k = 0; k < this->sliders.size(); k++)
    this->p_attr->value[k] = (int)this->sliders[k]->value();

  Q_EMIT this->value_changed();
}

} // namespace hesiod
