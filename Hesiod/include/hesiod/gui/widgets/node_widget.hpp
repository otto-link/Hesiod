/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <memory>

#include <QPointer>
#include <QWidget>

#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

class DataPreview; // forward decl.
class GraphNodeWidget;

// =====================================
// NodeInfoDialog
// =====================================

// generic widget embedded within the GraphicsNode
class NodeWidget : public QWidget
{
  Q_OBJECT
public:
  NodeWidget(std::weak_ptr<BaseNode>   model,
             QPointer<GraphNodeWidget> p_gnw,
             QWidget                  *parent = nullptr);

  QSize sizeHint() const override;

public slots:
  void on_compute_finished();
  void on_compute_started();

private:
  void setup_connections();

  std::weak_ptr<BaseNode>   model;
  QPointer<GraphNodeWidget> p_gnw;
  DataPreview              *data_preview;
};

} // namespace hesiod