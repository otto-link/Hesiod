/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGraphicsItem>
#include <QGridLayout>
#include <QPushButton>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

int float_to_slider_pos(float v, float min, float max, int slider_steps)
{
  return (int)((v - min) / (max - min) * (float)slider_steps);
}

QPointF qgraphicsitem_relative_coordinates(QGraphicsItem *item,
                                           QGraphicsView *view,
                                           QPointF       *p_global_pos)
{
  QPointF position = 0.5f * (item->boundingRect().topLeft() +
                             item->boundingRect().bottomRight());
  QPointF scene_pos = item->mapToScene(position);
  QPoint  viewport_pos = view->mapFromScene(scene_pos);
  QPointF view_pos = view->viewport()->mapToParent(viewport_pos);

  if (p_global_pos)
  {
    QPointF global_view_pos = view->mapToGlobal(QPoint(0, 0));
    *p_global_pos = QPointF(global_view_pos.x() + view_pos.x(),
                            global_view_pos.y() + view_pos.y());
  }

  return view_pos;
}

float slider_pos_to_float(int pos, float min, float max, int slider_steps)
{
  return min + (float)pos / (float)slider_steps * (max - min);
}

} // namespace hesiod
