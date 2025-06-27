/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <string>

#include <QApplication>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QWidget>

#include "hesiod/logger.hpp"

namespace hesiod
{

void render_widget_screenshot(QWidget           *widget,
                              const std::string &fname,
                              const QSize       &size)
{
  LOG->trace("render_widget_screenshot: fname {}", fname);

  // let the layout settle
  widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  widget->adjustSize();
  widget->updateGeometry();

  // get the final size of the widget after layout
  QSize actual_size = widget->sizeHint();
  if (!actual_size.isValid())
    actual_size = widget->size();

  // create a QPixmap or QImage for off-screen rendering
  QSize  render_size = size.isValid() ? size : actual_size;
  QImage image(render_size, QImage::Format_ARGB32_Premultiplied);
  image.fill(Qt::transparent);

  QPainter painter(&image);
  widget->render(&painter);
  painter.end();

  // crop to tight actual widget contents
  QRect  crop_rect(QPoint(0, 0), widget->sizeHint().boundedTo(render_size));
  QImage cropped = image.copy(crop_rect);

  // save the cropped image
  cropped.save(QString::fromStdString(fname));
}

} // namespace hesiod
