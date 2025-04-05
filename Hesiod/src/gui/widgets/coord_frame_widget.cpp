/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGraphicsRectItem>

#include "hesiod/gui/widgets/coord_frame_widget.hpp"
#include "hesiod/logger.hpp"

#define MAX_SIZE 40000

namespace hesiod
{

CoordFrameWidget::CoordFrameWidget(GraphManager *p_graph_manager, QWidget *parent)
    : QGraphicsView(parent), p_graph_manager(p_graph_manager)
{
  LOG->trace("CoordFrameWidget::CoordFrameWidget");

  // build layout
  this->setRenderHint(QPainter::Antialiasing);
  this->setRenderHint(QPainter::SmoothPixmapTransform);
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setDragMode(QGraphicsView::NoDrag);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // flip Y-axis
  QTransform transform;
  transform.scale(1.f, -1.f);
  this->setTransform(transform);

  this->setScene(new QGraphicsScene());
  this->scene()->setSceneRect(-MAX_SIZE, -MAX_SIZE, (MAX_SIZE * 2), (MAX_SIZE * 2));
  // this->setBackgroundBrush(QBrush(GN_STYLE->viewer.color_bg));

  // populate scene
  this->reset();
}

void CoordFrameWidget::add_frame(const std::string &id)
{
  std::map<std::string, std::shared_ptr<GraphEditor>> graphs = this->p_graph_manager
                                                                   ->get_graphs();

  hmap::Vec2<float> origin = graphs.at(id)->get_origin();
  hmap::Vec2<float> size = graphs.at(id)->get_size();
  float             angle = graphs.at(id)->get_rotation_angle();

  FrameItem *new_frame = new FrameItem(id,
                                       QPointF(origin.x, origin.y),
                                       QPointF(size.x, size.y),
                                       angle);
  this->scene()->addItem(new_frame);
  new_frame->set_z_depth(this->p_graph_manager->get_graph_order_index(id));

  // keep track of frame item ptr
  this->frames[id] = new_frame;

  // connections
  this->connect(new_frame,
                &FrameItem::has_changed,
                this,
                [this](const std::string & /* id */) { Q_EMIT this->has_changed(); });
}

void CoordFrameWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    this->setDragMode(QGraphicsView::ScrollHandDrag);

  QGraphicsView::mousePressEvent(event);
}

void CoordFrameWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    this->setDragMode(QGraphicsView::NoDrag);

  QGraphicsView::mouseReleaseEvent(event);
}

void CoordFrameWidget::on_zoom_to_content()
{
  QRectF bbox = this->scene()->itemsBoundingRect();

  // add a margin
  float margin_x = 0.1f * bbox.width();
  float margin_y = 0.1f * bbox.height();
  bbox.adjust(-margin_x, -margin_y, margin_x, margin_y);

  this->fitInView(bbox, Qt::KeepAspectRatio);
}

void CoordFrameWidget::remove_frame(const std::string &id)
{
  for (QGraphicsItem *item : this->scene()->items())
    if (FrameItem *p_frame = dynamic_cast<FrameItem *>(item))
      if (p_frame->get_id() == id)
      {
        this->scene()->removeItem(item);
        this->frames.erase(id);
      }

  this->update();
}

void CoordFrameWidget::reset()
{
  LOG->trace("CoordFrameWidget::reset");

  // store keys because the map is altered by remove_frame
  std::vector<std::string> keys = {};
  for (auto &[id, _] : this->frames)
    keys.push_back(id);

  for (auto &id : keys)
    this->remove_frame(id);

  // re-populate scene
  for (auto &id : this->p_graph_manager->get_graph_order())
  {
    LOG->trace("CoordFrameWidget::CoordFrameWidget: adding graph: {}", id);
    this->add_frame(id);
  }
}

void CoordFrameWidget::update_frames_z_depth()
{
  for (size_t k = 0; k < p_graph_manager->get_graph_order().size(); ++k)
  {
    const std::string id = p_graph_manager->get_graph_order()[k];
    this->frames.at(id)->set_z_depth((int)k);

    LOG->trace("CoordFrameWidget::update_frames_z_depth updated graph: {}, depth {}",
               id,
               k);
  }
}

void CoordFrameWidget::wheelEvent(QWheelEvent *event)
{
  const float factor = 1.2f;
  QPointF     mouse_scene_pos = this->mapToScene(event->position().toPoint());

  if (event->angleDelta().y() > 0)
    this->scale(factor, factor);
  else
    this->scale(1.f / factor, 1.f / factor);

  // adjust the view to maintain the zoom centered on the mouse position
  QPointF new_mouse_scene_pos = this->mapToScene(event->position().toPoint());
  QPointF delta = new_mouse_scene_pos - mouse_scene_pos;
  this->translate(delta.x(), delta.y());

  event->accept();
}

} // namespace hesiod
