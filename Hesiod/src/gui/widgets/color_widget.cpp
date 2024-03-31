/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QColorDialog>
#include <QHBoxLayout>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

ColorWidget::ColorWidget(ColorAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("Color attribute");

  QHBoxLayout *layout = new QHBoxLayout(this);

  this->label = new QLabel();

  int         r = (int)(255.f * this->p_attr->value[0]);
  int         g = (int)(255.f * this->p_attr->value[1]);
  int         b = (int)(255.f * this->p_attr->value[2]);
  int         a = (int)(255.f * this->p_attr->value[3]);
  std::string str = "QWidget {background-color: rgba(" + std::to_string(r) + ", " +
                    std::to_string(g) + ", " + std::to_string(b) + ", " +
                    std::to_string(a) + ");}";
  this->label->setStyleSheet(str.c_str());
  layout->addWidget(this->label);

  QPushButton *button = new QPushButton("Pick color");
  layout->addWidget(button);

  connect(button,
          &QPushButton::released,
          [this]()
          {
            QColorDialog color_dialog;
            color_dialog.setOption(QColorDialog::ShowAlphaChannel);
            QColor color = color_dialog.getColor();
            this->update_attribute(color);
          });

  this->setLayout(layout);
}

void ColorWidget::update_attribute(QColor color)
{
  this->p_attr->value[0] = (float)color.red() / 255.f;
  this->p_attr->value[1] = (float)color.green() / 255.f;
  this->p_attr->value[2] = (float)color.blue() / 255.f;
  this->p_attr->value[3] = (float)color.alpha() / 255.f;

  std::string str = "QWidget {background-color: rgba(" + std::to_string(color.red()) +
                    ", " + std::to_string(color.green()) + ", " +
                    std::to_string(color.blue()) + ", " + std::to_string(color.alpha()) +
                    ");}";
  this->label->setStyleSheet(str.c_str());

  Q_EMIT this->value_changed();
}

} // namespace hesiod
