/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QGridLayout>
#include <QVBoxLayout>

#include "hesiod/gui/widgets/graph_node_widget.hpp"

namespace hesiod
{

class GraphNode; // forward

// =====================================
// NodeSettingsWidget
// =====================================
class NodeSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  // --- Constructor and Setup ---
  NodeSettingsWidget(GraphNodeWidget *p_graph_node_widget, QWidget *parent = nullptr);

  void initialize_layout();

public slots:
  void on_node_selection_has_changed();

private:
  // --- Members ---
  GraphNodeWidget *p_graph_node_widget; // own by GraphManagerWidget
  QVBoxLayout     *layout;
  QGridLayout     *scroll_layout;
};

} // namespace hesiod