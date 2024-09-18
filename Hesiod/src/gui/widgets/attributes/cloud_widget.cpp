/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QPushButton>

#include "hesiod/gui/widgets/widgets.hpp"

namespace hesiod
{

CloudWidget::CloudWidget(CloudAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("Cloud attribute");

  QGridLayout *layout = new QGridLayout(this);

  PointEditorWidget *point_editor_widget = new PointEditorWidget(
      &this->p_attr->value.points,
      this);
  layout->addWidget(point_editor_widget, 0, 0, 1, 2);

  QPushButton *rand_button = new QPushButton("Randomize");
  layout->addWidget(rand_button, 1, 0);

  QPushButton *clear_button = new QPushButton("Clear");
  layout->addWidget(clear_button, 1, 1);

  this->setLayout(layout);

  connect(point_editor_widget,
          &PointEditorWidget::changed,
          this,
          &CloudWidget::value_changed);

  connect(rand_button,
          &QPushButton::released,
          [this, point_editor_widget]()
          {
            uint seed = time(NULL);
            this->p_attr->value.randomize(seed);
            point_editor_widget->update_scene();
            Q_EMIT this->value_changed();
          });

  connect(clear_button,
          &QPushButton::released,
          [this, point_editor_widget]()
          {
            this->p_attr->value.clear();
            point_editor_widget->clear_scene();
            Q_EMIT this->value_changed();
          });
}

void CloudWidget::update_attribute() { Q_EMIT this->value_changed(); }

} // namespace hesiod
