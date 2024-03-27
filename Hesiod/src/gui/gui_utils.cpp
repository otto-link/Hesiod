/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QLayout>
#include <QWidget>

namespace hesiod
{

// https://stackoverflow.com/questions/4857188

void clear_layout(QLayout *layout)
{
  if (layout == nullptr)
    return;

  QLayoutItem *item;

  while ((item = layout->takeAt(0)))
  {
    if (item->layout())
    {
      clear_layout(item->layout());
      delete item->layout();
    }
    if (item->widget())
    {
      delete item->widget();
    }
    delete item;
  }
}

} // namespace hesiod
