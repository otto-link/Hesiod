/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QWidget>

#include <QtNodes/AbstractGraphModel>

#include "DefaultHorizontalNodeGeometry.hpp"

#include "macrologger.h"

namespace QtNodes
{

void DefaultHorizontalNodeGeometry::recomputeSize(NodeId const nodeId) const
{
  unsigned int height = maxVerticalPortsExtent(nodeId);

  if (auto w = _graphModel.nodeData<QWidget *>(nodeId, NodeRole::Widget))
  {
    height += static_cast<unsigned int>(w->height()) - _portSpasing + 1;
  }

  QRectF const capRect = captionRect(nodeId);

  height += capRect.height();

  height += _portSpasing; // space above caption
  height += _portSpasing; // space below caption

  unsigned int inPortWidth = maxPortsTextAdvance(nodeId, PortType::In);
  unsigned int outPortWidth = maxPortsTextAdvance(nodeId, PortType::Out);

  unsigned int width = inPortWidth + outPortWidth + 4 * _portSpasing;

  if (auto w = _graphModel.nodeData<QWidget *>(nodeId, NodeRole::Widget))
  {
    // width += w->width();
    width = std::max(width, (uint)w->width() + 2);
  }

  width = std::max(width, static_cast<unsigned int>(capRect.width()) + 2 * _portSpasing);

  QSize size(width, height);

  _graphModel.setNodeData(nodeId, NodeRole::Size, size);
}

QPointF DefaultHorizontalNodeGeometry::widgetPosition(NodeId const nodeId) const
{
  QSize size = _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);

  unsigned int captionHeight = captionRect(nodeId).height();

  if (auto w = _graphModel.nodeData<QWidget *>(nodeId, NodeRole::Widget))
  {
    return QPointF(1.f, captionHeight + maxVerticalPortsExtent(nodeId) + _portSpasing);

    // // If the widget wants to use as much vertical space as possible,
    // // place it immediately after the caption.
    // if (w->sizePolicy().verticalPolicy() & QSizePolicy::ExpandFlag)
    // {
    //   return QPointF(0.f,
    //                  captionHeight + maxVerticalPortsExtent(nodeId));
    // }
    // else
    // {
    //   return QPointF(0.f,
    //                  (captionHeight + size.height() - w->height()) / 2.0 +
    //                      +maxVerticalPortsExtent(nodeId));
    // }
  }
  return QPointF();
}

} // namespace QtNodes
