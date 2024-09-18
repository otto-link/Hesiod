
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "hesiod/gui/widgets/widgets.hpp"

namespace hesiod
{

PointEditorWidget::PointEditorWidget(std::vector<hmap::Point> *p_points, QWidget *parent)
    : QGraphicsView(parent), p_points(p_points)
{
  this->width = 256;
  this->height = 256;

  this->setFixedSize(width + 2, height + 2);

  this->setWindowTitle("2D Point Editor");
  this->scene = new QGraphicsScene(parent);
  this->scene->setSceneRect(0, 0, width, height);

  this->scene->addRect(QRectF(0.f, 0.f, (float)width, (float)height),
                       QPen(Qt::black),
                       QBrush(Qt::gray));

  // set points
  this->update_scene();

  // set view
  this->setScene(this->scene);
  this->setMinimumSize(width, height);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // TODO add points already in the Cloud object
}

void PointEditorWidget::add_point(QPointF event_pos, float point_value)
{
  int     radius = 24;
  QPoint  pos = event_pos.toPoint() - QPoint(radius / 2, radius / 2);
  QPointF new_point = this->mapFromGlobal(pos);

  QGraphicsEllipseItem *ellipse_item = this->scene->addEllipse(
      QRectF(new_point.x(), new_point.y(), radius, radius),
      QPen(Qt::black),
      QBrush(Qt::black));
  ellipse_item->setData(this->data_key, point_value);
  ellipse_item->setFlag(QGraphicsItem::ItemIsMovable);
}

void PointEditorWidget::clear_scene()
{
  // only remove the ellipse objects
  QList<QGraphicsItem *> all_items = this->scene->items();

  for (QGraphicsItem *item : all_items)
  {
    QGraphicsItem *ellipse_item = dynamic_cast<QGraphicsEllipseItem *>(item);
    if (ellipse_item && ellipse_item->type() == QGraphicsEllipseItem::Type)
    {
      this->scene->removeItem(item);
      delete item;
    }
  }
}

void PointEditorWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
  QGraphicsView::mouseDoubleClickEvent(event);
  this->add_point(event->globalPosition());
  this->update_points();
}

void PointEditorWidget::mouseMoveEvent(QMouseEvent *event)
{
  QGraphicsView::mouseMoveEvent(event);

  if (this->moving_point && event->buttons() & Qt::LeftButton)
  {
    int x = std::clamp(event->pos().x(),
                       (int)this->scene->sceneRect().left(),
                       (int)this->scene->sceneRect().right());
    int y = std::clamp(event->pos().y(),
                       (int)this->scene->sceneRect().top(),
                       (int)this->scene->sceneRect().bottom());

    QPointF new_pos = mapToScene(QPoint(x, y)) + this->offset;

    this->moving_point->setPos(new_pos);
  }
}

void PointEditorWidget::mousePressEvent(QMouseEvent *event)
{
  QGraphicsView::mousePressEvent(event);

  if (event->button() == Qt::LeftButton)
  {
    QPointF        scene_pos = mapToScene(event->pos());
    QGraphicsItem *item = this->scene->itemAt(scene_pos, QTransform());
    if (item && item->type() == QGraphicsEllipseItem::Type)
    {
      this->moving_point = dynamic_cast<QGraphicsEllipseItem *>(item);
      if (this->moving_point)
      {
        this->offset = this->moving_point->pos() - scene_pos;
        this->moving_point->setBrush(Qt::blue);
        return;
      }
    }
  }

  if (event->button() == Qt::RightButton)
  {
    QPointF        scene_pos = mapToScene(event->pos());
    QGraphicsItem *item = this->scene->itemAt(scene_pos, QTransform());
    if (item && item->type() == QGraphicsEllipseItem::Type)
    {
      this->scene->removeItem(item);
      this->update_points();
      return;
    }
  }

  this->moving_point = nullptr;
}

void PointEditorWidget::mouseReleaseEvent(QMouseEvent *event)
{
  QGraphicsView::mouseReleaseEvent(event);

  if (this->moving_point)
  {
    this->moving_point->setBrush(Qt::black);
    this->moving_point = nullptr;

    this->update_points();
  }
}

void PointEditorWidget::update_points()
{
  QList<QGraphicsItem *> items = this->scene->items();

  this->p_points->clear();

  for (QGraphicsItem *item : items)
    if (item->type() == QGraphicsEllipseItem::Type)
    {
      QGraphicsEllipseItem *ellipse_item = dynamic_cast<QGraphicsEllipseItem *>(item);
      if (ellipse_item)
      {
        QPointF position = qgraphicsitem_relative_coordinates(ellipse_item, this);
        float   x = position.x() / (float)this->width;
        float   y = 1.f - position.y() / (float)this->height;
        this->p_points->push_back(
            hmap::Point(x, y, item->data(this->data_key).toFloat()));
      }
    }

  std::reverse(this->p_points->begin(), this->p_points->end());

  Q_EMIT this->changed();
}

void PointEditorWidget::update_scene()
{
  this->clear_scene();
  QPointF global_view_pos = this->mapToGlobal(QPoint(0, 0));

  for (auto &p : *this->p_points)
  {
    QPointF pos = QPointF(p.x * this->width + global_view_pos.x(),
                          (1.f - p.y) * this->height + global_view_pos.y());

    this->add_point(pos, p.v);
  }
}

void PointEditorWidget::wheelEvent(QWheelEvent *event)
{
  // QPointF        scene_pos = mapToScene(event->position());
  // QGraphicsItem *item = this->scene->itemAt(scene_pos, QTransform());
  // if (item && item->type() == QGraphicsEllipseItem::Type)
  // {
  //   QGraphicsEllipseItem *ellipse_item = dynamic_cast<QGraphicsEllipseItem *>(item);
  //   if (ellipse_item)
  //   {
  //     float delta = 0.01f;

  //     if (event->pixelDelta().x() > 0)
  //       ellipse_item->setData(this->data_key,
  //                             ellipse_item->data(this->data_key).toFloat() + delta);
  //     else
  //       ellipse_item->setData(this->data_key,
  //                             ellipse_item->data(this->data_key).toFloat() - delta);

  //     this->update_points();
  //   }
  // }
}

} // namespace hesiod
