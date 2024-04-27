/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

ColorbarWidget::ColorbarWidget(QWidget *parent) : QLabel(parent)
{
  // this->setMinimumHeight(24);
  // position / r / g / b / a
  this->colors = {{0.f, 0.f, 0.f, 0.f, 1.f}};
}

ColorbarWidget::ColorbarWidget(std::vector<std::vector<float>> colors, QWidget *parent)
    : QLabel(parent), colors(colors)
{
  // this->setMinimumHeight(24);
}

void ColorbarWidget::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);

  // define the gradient
  QLinearGradient gradient(0, 0, this->width(), this->height());

  for (auto &c : this->colors)
    gradient.setColorAt(c[0], QColor::fromRgbF(c[1], c[2], c[3], c[4]));

  // draw rectangle
  QBrush brush(gradient);
  painter.setBrush(brush);
  painter.setPen(Qt::transparent); // no border
  painter.fillRect(this->rect(), brush);

  // base paint event
  QWidget::paintEvent(event);
}

void ColorbarWidget::update_colors(std::vector<std::vector<float>> new_colors)
{
  this->colors = new_colors;
  this->repaint();
}

} // namespace hesiod
