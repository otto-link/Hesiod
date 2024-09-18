/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QSlider>

#include "hesiod/gui/widgets/widgets.hpp"

namespace hesiod
{

VecFloatWidget::VecFloatWidget(VecFloatAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("VecFloat attribute");

  QGridLayout *layout = new QGridLayout(this);

  int row = 0;
  int col = 0;

  for (auto &v : this->p_attr->value)
  {
    this->labels.push_back(new QLabel());
    this->labels.back()->setAlignment(Qt::AlignCenter);
    this->labels.back()->setText(QString().asprintf(this->p_attr->fmt.c_str(), v));
    layout->addWidget(this->labels.back(), row, col++, Qt::AlignHCenter);

    this->sliders.push_back(new QSlider(Qt::Vertical));
    this->sliders.back()->setRange(0, HSD_SLIDER_STEPS);
    this->sliders.back()->setSingleStep(0);
    this->sliders.back()->setPageStep(0);
    int pos = float_to_slider_pos(v,
                                  this->p_attr->vmin,
                                  this->p_attr->vmax,
                                  HSD_SLIDER_STEPS);
    this->sliders.back()->setValue(pos);
    layout->addWidget(this->sliders.back(), row + 1, col++);
    col++;
  }

  for (size_t k = 0; k < this->sliders.size(); k++)
  {
    // only display value but no signal
    connect(this->sliders[k],
            &QSlider::valueChanged,
            [this, k]()
            {
              float v = slider_pos_to_float(this->sliders[k]->value(),
                                            this->p_attr->vmin,
                                            this->p_attr->vmax,
                                            HSD_SLIDER_STEPS);
              this->labels[k]->setText(QString().asprintf(this->p_attr->fmt.c_str(), v));
            });

    // signal change
    connect(this->sliders[k],
            &QSlider::sliderReleased,
            this,
            &VecFloatWidget::update_attribute);
  }

  // row += 2;
  // TODO add vector size change (plus / minus buttons)

  this->setLayout(layout);
}

void VecFloatWidget::update_attribute()
{
  for (size_t k = 0; k < this->sliders.size(); k++)
    this->p_attr->value[k] = slider_pos_to_float(this->sliders[k]->value(),
                                                 this->p_attr->vmin,
                                                 this->p_attr->vmax,
                                                 HSD_SLIDER_STEPS);

  Q_EMIT this->value_changed();
}

} // namespace hesiod
