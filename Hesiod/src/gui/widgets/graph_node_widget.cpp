/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_node.hpp"

namespace hesiod
{

GraphNodeWidget::GraphNodeWidget(GraphNode *p_graph_node, QWidget *parent)
  : QWidget(parent), p_graph_node(p_graph_node)
{
  LOG->trace("GraphNodeWidget::GraphNodeWidget");
}

} // namespace hesiod
