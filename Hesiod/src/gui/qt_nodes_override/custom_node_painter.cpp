/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#ifdef HSD_OS_LINUX

#include <QtNodes/AbstractGraphModel>
#include <QtNodes/AbstractNodePainter>
#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DefaultNodePainter>
#include <QtNodes/internal/AbstractNodeGeometry.hpp>
#include <QtNodes/internal/NodeGraphicsObject.hpp>

#include "macrologger.h"

#include "hesiod/gui/node_category_color.hpp"

namespace QtNodes
{

void DefaultNodePainter::drawNodeCaption(QPainter *painter, NodeGraphicsObject &ngo) const
{
  AbstractGraphModel   &model = ngo.graphModel();
  NodeId const          nodeId = ngo.nodeId();
  AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

  QString const name = model.nodeData(nodeId, NodeRole::Caption).toString();

  // --- colored background

  QSize        size = geometry.size(nodeId);
  QRectF       boundary(0, 0, size.width(), size.height());
  double const radius = 3;

  // filled half-rounded rectangle behind the caption
  {

    // crawl back to the node category...
    QtNodes::DataFlowGraphModel &graph_model = (QtNodes::DataFlowGraphModel &)model;
    std::shared_ptr<NodeDelegateModelRegistry> registry = graph_model.dataModelRegistry();
    std::unordered_map<QString, QString>
        node_type_map = registry->registeredModelsCategoryAssociation();

    std::string node_category = node_type_map.at(name).toStdString();
    int         pos = node_category.find("/");
    std::string main_category = node_category.substr(0, pos);

    QColor fill_color = QColor(255, 0, 0);

    if (hesiod::node_category_color.contains(main_category))
      fill_color = hesiod::node_category_color.at(main_category);

    painter->setBrush(fill_color);
    painter->setPen(Qt::NoPen);

    QRect rect(1, 0, size.width() - 1, 32);

    QPainterPath path;
    path.moveTo(rect.left(), rect.bottom());
    path.lineTo(rect.left(), rect.top() + radius);
    path.arcTo(rect.left(), rect.top(), radius * 2, radius * 2, 180, -90);
    path.lineTo(rect.right() - radius, rect.top());
    path.arcTo(rect.right() - radius * 2, rect.top(), radius * 2, radius * 2, 90, -90);
    path.lineTo(rect.right(), rect.bottom());
    path.closeSubpath();

    painter->drawPath(path);
  }

  // --- caption

  if (!model.nodeData(nodeId, NodeRole::CaptionVisible).toBool())
    return;

  QFont f = painter->font();
  f.setBold(true);

  QPointF position = geometry.captionPosition(nodeId);

  QJsonDocument json = QJsonDocument::fromVariant(
      model.nodeData(nodeId, NodeRole::Style));
  NodeStyle nodeStyle(json.object());

  painter->setFont(f);
  painter->setPen(nodeStyle.FontColor);
  painter->drawText(position, name);

  f.setBold(false);
  painter->setFont(f);
}

} // namespace QtNodes

#endif
