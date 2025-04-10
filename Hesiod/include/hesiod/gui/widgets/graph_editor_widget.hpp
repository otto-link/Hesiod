/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file graph_editor_widget.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include <QTabWidget>
#include <QWidget>

#include "nlohmann/json.hpp"

// TODO - rename as GraphTabsWidget

namespace hesiod
{

class GraphManager; // forward
class GraphNodeWidget;

// =====================================
// GraphEditorWidget
// =====================================
class GraphEditorWidget : public QWidget
{
  Q_OBJECT

public:
  GraphEditorWidget(GraphManager *p_graph_manager, QWidget *parent = nullptr);

  void clear();

  // --- Serialization ---
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;
  // void           load_from_file(const std::string &fname);
  // bool           save_to_file(const std::string &fname) const;

signals:
  void has_been_cleared(const std::string &graph_id);
  void new_node_created(const std::string &graph_id, const std::string &id);
  void node_deleted(const std::string &graph_id, const std::string &id);

public slots:
  // --- From GraphNodeWidget signals ---
  void on_has_been_cleared(const std::string &graph_id);
  void on_new_node_created(const std::string &graph_id, const std::string &id);
  void on_node_deleted(const std::string &graph_id, const std::string &id);

  void set_selected_tab(const std::string &graph_id);
  void update_receive_nodes_tag_list();
  void update_tab_widget();

private:
  // --- Members ---
  GraphManager                            *p_graph_manager; // own by MainWindow
  QTabWidget                              *tab_widget;      // own by this
  std::map<std::string, GraphNodeWidget *> graph_node_widget_map;
};

} // namespace hesiod