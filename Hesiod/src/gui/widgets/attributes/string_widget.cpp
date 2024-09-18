/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QHBoxLayout>

#include "hesiod/gui/widgets/widgets.hpp"

namespace hesiod
{

StringWidget::StringWidget(StringAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("String attribute");

  QHBoxLayout *layout = new QHBoxLayout(this);

  this->text_edit = new QPlainTextEdit();
  this->text_edit->setPlainText(this->p_attr->value.c_str());

  connect(this->text_edit,
          &QPlainTextEdit::textChanged,
          [this]() { this->update_attribute(); });

  layout->addWidget(this->text_edit);
  this->setLayout(layout);
}

void StringWidget::update_attribute()
{
  this->p_attr->value = this->text_edit->toPlainText().toStdString();
  Q_EMIT this->value_changed();
}

} // namespace hesiod
