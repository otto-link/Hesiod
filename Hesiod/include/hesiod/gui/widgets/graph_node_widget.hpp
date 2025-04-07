/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file graph_node_widget.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include <QWidget>

namespace hesiod
{

class GraphNode; // forward

// =====================================
// GraphNodeWidget
// =====================================
class GraphNodeWidget : public QWidget
{
  Q_OBJECT

public:
  GraphNodeWidget(GraphNode *p_graph_node, QWidget *parent = nullptr);

private:
  // --- Members ---
  GraphNode *p_graph_node; // own by GraphManager
};

} // namespace hesiod