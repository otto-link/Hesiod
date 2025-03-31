/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QScrollArea>

#include "attributes/widgets/attributes_widget.hpp"

#include "hesiod/gui/widgets/config_widget.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

ConfigWidget::ConfigWidget(QWidget *parent) : QWidget(parent)
{
  LOG->trace("ConfigWidget::ConfigWidget");

  // -- build layout

  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QGridLayout *layout = new QGridLayout(this);
  int          row = 0;

  if (this->flat_layout)
  {
    auto *attr_widgets = new attr::AttributesWidget(&CFG->get_attr());

    // change the attribute widget layout spacing a posteriori
    QVBoxLayout *retrieved_layout = qobject_cast<QVBoxLayout *>(attr_widgets->layout());
    if (retrieved_layout)
    {
      retrieved_layout->setSpacing(0);
      retrieved_layout->setContentsMargins(0, 0, 0, 0);
    }

    // add to scroll area
    QScrollArea *scroll_area = new QScrollArea;
    scroll_area->setWidget(attr_widgets);
    scroll_area->setWidgetResizable(true);
    layout->addWidget(scroll_area, row++, 0);
  }

  this->setLayout(layout);
}

} // namespace hesiod
