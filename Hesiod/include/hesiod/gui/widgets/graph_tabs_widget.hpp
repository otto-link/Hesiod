/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <memory>

#include <QTabWidget>
#include <QWidget>

#include "nlohmann/json.hpp"

#include "hesiod/gui/widgets/node_settings_widget.hpp"

namespace hesiod
{

class GraphManager; // forward
class GraphNodeWidget;

// =====================================
// GraphTabsWidget
// =====================================
class GraphTabsWidget : public QWidget
{
  Q_OBJECT

public:
  GraphTabsWidget(std::weak_ptr<GraphManager> p_graph_manager, QWidget *parent = nullptr);

  void clear();
  void set_show_node_settings_widget(bool new_state);

  // --- Serialization ---
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

  std::string get_selected_graph_id() const;

  QSize sizeHint() const override;

signals:
  // --- UI
  void has_been_cleared(const std::string &graph_id);
  void has_changed();
  void new_node_created(const std::string &graph_id, const std::string &id);
  void node_deleted(const std::string &graph_id, const std::string &id);

  // --- Graph
  void update_started();
  void update_finished();

public slots:
  // --- From GraphNodeWidget signals ---
  void on_copy_buffer_has_changed(const nlohmann::json &new_json);
  void on_has_been_cleared(const std::string &graph_id);
  void on_new_node_created(const std::string &graph_id, const std::string &id);
  void on_node_deleted(const std::string &graph_id, const std::string &id);
  void on_textures_request(const std::vector<std::string> &texture_paths);

  void set_selected_tab(const std::string &graph_id);
  void show_viewport();
  void update_receive_nodes_tag_list();
  void update_tab_widget();
  void zoom_to_content();

private:
  // --- Members ---
  std::weak_ptr<GraphManager>                      p_graph_manager; // own by MainWindow
  QTabWidget                                      *tab_widget;      // own by this
  std::map<std::string, QPointer<GraphNodeWidget>> graph_node_widget_map;
  std::map<std::string, NodeSettingsWidget *>      node_settings_widget_map;
  bool                                             show_node_settings_widget;
};

} // namespace hesiod