/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QHBoxLayout>
#include <QPushButton>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

MapEnumWidget::MapEnumWidget(MapEnumAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("MapEnum attribute");

  QHBoxLayout *layout = new QHBoxLayout(this);

  this->combobox = new QComboBox();
  QStringList items;
  for (auto &[key, dummy] : this->p_attr->get_map())
    this->combobox->addItem(key.c_str());

  if (this->p_attr->get_map().contains(this->p_attr->choice))
    this->combobox->setCurrentText(QString::fromStdString(this->p_attr->choice));
  else
    this->combobox->setCurrentText(this->p_attr->get_map().begin()->first.c_str());

  layout->addWidget(this->combobox);

  connect(this->combobox,
          QOverload<int>::of(&QComboBox::currentIndexChanged),
          this,
          &MapEnumWidget::update_attribute);

  this->setLayout(layout);
}

void MapEnumWidget::update_attribute()
{
  this->p_attr->choice = this->combobox->currentText().toStdString();
  Q_EMIT this->value_changed();
}

} // namespace hesiod
