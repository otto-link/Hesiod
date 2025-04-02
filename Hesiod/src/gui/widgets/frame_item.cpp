/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGraphicsSceneMouseEvent>

#include "hesiod/gui/widgets/coord_frame_widget.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

FrameItem::FrameItem(const std::string &id, QPointF origin, QPointF size, float angle)
    : QGraphicsRectItem(), id(id), origin(origin), size(size), angle(angle)
{
  this->setFlag(QGraphicsItem::ItemIsSelectable, true);
  this->setFlag(QGraphicsItem::ItemIsMovable, true);
  this->setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, false);
  this->setFlag(QGraphicsItem::ItemIsFocusable, true);
  this->setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
  this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
  this->setAcceptHoverEvents(true);
  this->setOpacity(1.f);
  this->setZValue(0);

  this->update_item_geometry();
}

void FrameItem::get_frame_parameter(QPointF &origin_out,
                                    QPointF &size_out,
                                    float   &angle_out) const
{
  origin_out = this->origin;
  size_out = this->size;
  angle_out = this->angle;
}

void FrameItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  this->is_hovered = true;
  this->set_is_rotated(false);
  this->set_is_resized(false);
  this->update();

  QGraphicsRectItem::hoverEnterEvent(event);
}

void FrameItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  this->is_hovered = false;
  this->set_is_rotated(false);
  this->set_is_resized(false);
  this->update();

  QGraphicsRectItem::hoverLeaveEvent(event);
}

void FrameItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
  this->set_is_rotated(this->rotation_rect.contains(event->pos()));
  this->set_is_resized(this->resize_rect.contains(event->pos()));
}

QVariant FrameItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
  if (change == QGraphicsItem::ItemPositionChange)
  {
    QPointF new_pos = value.toPointF();
    this->origin = new_pos / COORD_SCALE;

    Q_EMIT this->has_changed(this->id);
  }

  return QGraphicsRectItem::itemChange(change, value);
}

void FrameItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if (this->is_rotated && (event->buttons() & Qt::LeftButton))
  {
    // adjust the angle to center the rectangle on the mouse cursor
    QPointF pos = event->pos();
    QPointF center = this->rotation_rect.center();

    float pos_angle = std::atan2(pos.y(), pos.x());
    float center_angle = std::atan2(center.y(), center.x());
    float delta = (pos_angle - center_angle) / 3.14159f * 180.f;

    this->set_angle(this->angle + delta);
  }
  else if (this->is_resized && (event->buttons() & Qt::LeftButton))
  {
    QPointF pos = event->pos();
    QPointF center = this->resize_rect.center();
    QPointF delta = pos - center;

    // 1:1 aspect ratio scaling if Shift
    if (event->modifiers() & Qt::ShiftModifier)
    {
      float dmax = std::max(delta.x(), delta.y());
      delta = QPointF(dmax, dmax);
    }

    // prevent negative sizes
    delta /= COORD_SCALE;
    QPointF new_size = this->size + QPointF(delta.x(), delta.y());
    new_size = QPointF(std::max(0.f, (float)new_size.x()),
                       std::max(0.f, (float)new_size.y()));

    this->set_size(new_size);
  }

  QGraphicsRectItem::mouseMoveEvent(event);
}

void FrameItem::paint(QPainter                       *painter,
                      const QStyleOptionGraphicsItem *option,
                      QWidget                        *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  QRect base_rect(0.f, 0.f, COORD_SCALE * this->size.x(), COORD_SCALE * this->size.y());

  // colors
  QColor color_bg(102, 102, 102, 127);
  QColor color_border(0, 0, 0, 255);
  QColor color_hovered(255, 255, 255, 255);

  // background
  if (!this->image.isNull())
  {
    QImage scaled_image = this->image.scaled(this->rect().size().toSize(),
                                             Qt::IgnoreAspectRatio,
                                             Qt::SmoothTransformation);
    painter->drawImage(this->rect(), scaled_image);
  }
  else
  {
    painter->setBrush(QBrush(color_bg));
    painter->setPen(Qt::NoPen);
    painter->drawRect(base_rect);
  }

  // contour
  painter->setBrush(Qt::NoBrush);

  if (this->is_hovered)
    painter->setPen(QPen(color_hovered, 1.f));
  else
    painter->setPen(QPen(color_border, 1.f));

  painter->drawRect(base_rect);

  // corners
  painter->drawRect(this->rotation_rect);
  painter->drawRect(this->resize_rect);

  // QGraphicsRectItem::paint(painter, option, widget);
}

void FrameItem::set_angle(float new_angle)
{
  this->angle = new_angle;

  this->update_item_geometry();
}

void FrameItem::set_geometry(QPointF new_origin, QPointF new_size, float new_angle)
{
  this->origin = new_origin;
  this->size = new_size;
  this->angle = new_angle;

  this->update_item_geometry();
}

void FrameItem::set_is_resized(bool new_is_resized)
{
  this->is_resized = new_is_resized;

  if (this->is_resized)
  {
    this->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setCursor(Qt::SizeBDiagCursor);
  }
  else if (!this->is_rotated)
  {
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setCursor(Qt::ArrowCursor);
  }
}

void FrameItem::set_is_rotated(bool new_is_rotated)
{
  this->is_rotated = new_is_rotated;

  if (this->is_rotated)
  {
    this->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setCursor(Qt::CrossCursor);
  }
  else if (!this->is_resized)
  {
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setCursor(Qt::ArrowCursor);
  }
}

void FrameItem::set_origin(QPointF new_origin)
{
  this->origin = new_origin;
  this->update_item_geometry();
}

void FrameItem::set_size(QPointF new_size)
{
  this->size = new_size;
  this->update_item_geometry();
}

void FrameItem::set_z_depth(int new_z_depth)
{
  this->z_depth = new_z_depth;
  this->setZValue(static_cast<qreal>(this->z_depth));
  this->update();
}

void FrameItem::update_item_geometry()
{
  this->setRect(0.f, 0.f, COORD_SCALE * this->size.x(), COORD_SCALE * this->size.y());
  this->setPos(COORD_SCALE * this->origin);
  this->setRotation(this->angle);

  const std::string tool_tip = this->id;
  // + ": " + std::to_string(this->origin.x()) + ", " +
  //                              std::to_string(this->origin.y()) + " " +
  //                              std::to_string(this->size.x()) + "x" +
  //                              std::to_string(this->size.y()) + " " +
  //                              std::to_string(this->angle) + "°";
  this->setToolTip(tool_tip.c_str());

  // corners (warning, y-scale is reversed...)
  this->rotation_rect = QRectF(this->rect().topRight() - QPointF(this->handle_size, 0.f),
                               QSizeF(this->handle_size, this->handle_size));

  this->resize_rect = QRectF(this->rect().bottomRight() -
                                 QPointF(this->handle_size, this->handle_size),
                             QSizeF(this->handle_size, this->handle_size));

  this->update();

  Q_EMIT this->has_changed(this->id);
}

} // namespace hesiod
