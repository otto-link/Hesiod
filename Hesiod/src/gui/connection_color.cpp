/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QColor>

#include <QtNodes/ConnectionStyle>

namespace QtNodes
{

QColor ConnectionStyle::normalColor(QString typeId) const
{
  // Dracula theme
  if (QString::compare(typeId, "ArrayData") == 0)
    return QColor::fromRgb(255, 121, 198);
  else if (QString::compare(typeId, "CloudData") == 0)
    return QColor::fromRgb(139, 233, 253);
  else if (QString::compare(typeId, "HeightMapData") == 0)
    return QColor::fromRgb(248, 248, 242);
  else if (QString::compare(typeId, "HeightMapRGBAData") == 0)
    return QColor::fromRgb(255, 184, 108);
  else if (QString::compare(typeId, "PathData") == 0)
    return QColor::fromRgb(80, 250, 123);

  return QColor::fromRgb(255, 0, 0);
}

} // namespace QtNodes
