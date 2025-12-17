/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QButtonGroup>
#include <QPointer>

#include "hesiod/gui/widgets/graph_node_widget.hpp"

namespace hesiod
{

// =====================================
// GraphToolbar
// =====================================
class GraphToolbar : public QWidget
{
public:
  GraphToolbar(QPointer<GraphNodeWidget> p_graph_node_widget, QWidget *parent = nullptr);

private slots:
  void on_resolution_button_clicked(QAbstractButton *button);
  void sync_resolution_from_config();

private:
  void setup_layout();

  // --- Members ---
  QPointer<GraphNodeWidget> p_graph_node_widget; // own by GraphManagerWidget
  QButtonGroup             *resolution_group = nullptr;
};

} // namespace hesiod
