/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QFileDialog>
#include <QHBoxLayout>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

FilenameWidget::FilenameWidget(FilenameAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("Filename attribute");

  QHBoxLayout *layout = new QHBoxLayout(this);

  this->button = new QPushButton(QString::fromStdString(this->p_attr->value));
  layout->addWidget(this->button);

  connect(button,
          &QPushButton::released,
          [this]()
          {
            QString fname = QFileDialog::getOpenFileName(this,
                                                         this->p_attr->label.c_str(),
                                                         ".",
                                                         this->p_attr->filter.c_str());
            this->p_attr->value = fname.toStdString();
            this->update_attribute();
          });

  this->setLayout(layout);
}

void FilenameWidget::update_attribute()
{
  std::string text = "";
  size_t      max_size = HSD_FILENAME_DISPLAY_MAX_SIZE;
  if (this->p_attr->value.size() > max_size)
    text = "..." +
           this->p_attr->value.substr(this->p_attr->value.size() - max_size, max_size);
  else
    text = this->p_attr->value;

  this->button->setText(QString::fromStdString(text));
  Q_EMIT this->value_changed();
}

} // namespace hesiod
