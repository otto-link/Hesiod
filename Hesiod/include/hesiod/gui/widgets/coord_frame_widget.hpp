/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file coord_frame_widget.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QPixmap>

#include "hesiod/graph_manager.hpp"

#define COORD_SCALE 1e2f

namespace hesiod
{

class FrameItem : public QObject, public QGraphicsRectItem
{
  Q_OBJECT

public:
  FrameItem(const std::string &id, QPointF origin, QPointF size, float angle);

  void paint(QPainter                       *painter,
             const QStyleOptionGraphicsItem *option,
             QWidget                        *widget = nullptr) override;

  std::string get_id() const { return this->id; }

  void get_frame_parameter(QPointF &origin_out,
                           QPointF &size_out,
                           float   &angle_out) const;

  void set_angle(float new_angle);

  void set_background_image(const QImage &new_image);

  void set_geometry(QPointF new_origin, QPointF new_size, float new_angle);

  void set_is_resized(bool new_is_resized);

  void set_is_rotated(bool new_is_rotated);

  void set_origin(QPointF new_origin);

  void set_size(QPointF new_size);

  void set_z_depth(int new_z_depth);

  void update_item_geometry();

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

  void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

  QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

Q_SIGNALS:
  void has_changed(const std::string &id);

private:
  std::string id = "UNDEFINED";
  QPointF     origin;
  QPointF     size;
  float       angle;
  int         z_depth;

  QPixmap pixmap = QPixmap();

  bool is_hovered = false;
  bool is_rotated = false;
  bool is_resized = false;

  // corners
  QRectF rotation_rect;
  QRectF resize_rect;
  float  handle_size = 0.2f * COORD_SCALE;
};

class CoordFrameWidget : public QGraphicsView
{
  Q_OBJECT

public:
  CoordFrameWidget(GraphManager *p_graph_manager, QWidget *parent = nullptr);

  void add_frame(const std::string &id);

  FrameItem *get_frame_ref(const std::string &id) { return this->frames.at(id); }

  void remove_frame(const std::string &id);

public Q_SLOTS:
  void on_zoom_to_content();

  void update_frames_z_depth();

  void reset();

Q_SIGNALS:
  void has_changed();

protected:
  void mousePressEvent(QMouseEvent *event) override;

  void mouseReleaseEvent(QMouseEvent *event) override;

  void wheelEvent(QWheelEvent *event) override;

private:
  GraphManager *p_graph_manager = nullptr;

  std::map<std::string, FrameItem *> frames;
};

} // namespace hesiod