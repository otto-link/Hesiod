/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QHBoxLayout>
#include <QPushButton>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

CloudWidget::CloudWidget(CloudAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("Cloud attribute");

  QHBoxLayout *layout = new QHBoxLayout(this);

  PointEditorWidget *point_editor_widget = new PointEditorWidget(
      &this->p_attr->value.points,
      this);

  layout->addWidget(point_editor_widget);

  this->setLayout(layout);

  connect(point_editor_widget,
          &PointEditorWidget::changed,
          this,
          &CloudWidget::value_changed);
}

void CloudWidget::update_attribute() { Q_EMIT this->value_changed(); }

} // namespace hesiod
