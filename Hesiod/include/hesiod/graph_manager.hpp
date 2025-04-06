/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file graph_manager.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 *
 * @copyright Copyright (c) 2025
 */
#pragma once
#include <QObject>
#include <QPushButton>
#include <QTabWidget>

#include "hesiod/graph_editor.hpp"

#include <filesystem> // must be here,
                      // https://bugreports.qt.io/browse/QTBUG-73263

namespace hesiod
{

// - TODO add global default config widget
// - TODO handle batch mode
// - TODO import 1 graph
// - TODO multiple node deletion only calls one connect
// - TODO add setting to allow broadcast within the same graph
// - TODO clean-up load/save with MainWindow
// - TODO keybindings to switch between editors
// - add Graph file menu

struct BroadcastParam
{
  const hmap::Terrain   *t_source = nullptr;
  const hmap::Heightmap *p_h = nullptr;
};

class GraphManager : public QObject
{
  Q_OBJECT

public:
  GraphManager() = default;

  GraphManager(bool headless) : headless(headless) {}

  std::string add_graph_editor(const std::shared_ptr<GraphEditor> &p_graph_editor,
                               const std::string                  &id = "");

  void clear();

  void dump() const;

  GraphEditor *get_graph_ref_by_id(const std::string &id);

  int get_graph_order_index(const std::string &id);

  const std::vector<std::string> &get_graph_order() { return this->graph_order; }

  const std::map<std::string, std::shared_ptr<GraphEditor>> &get_graphs()
  {
    return this->graphs;
  }

  bool get_headless() const { return this->headless; }

  bool is_graph_above(const std::string &graph_id, const std::string &ref_graph_id);

  bool is_graph_id_available(const std::string &id);

  void json_from(nlohmann::json const &json);

  nlohmann::json json_to() const;

  // graph manager has its own load/save from file used for headless mode. In GUI mode,
  // load/save are called by the MainWindow instance
  void load_from_file(const std::string &fname);

  void remove_graph_editor(const std::string &id);

  void save_to_file(const std::string &fname) const;

  void set_graph_order(const std::vector<std::string> &new_graph_order);

  void set_selected_tab(const std::string &id);

  void set_tab_widget(QTabWidget *new_tab_widget) { this->tab_widget = new_tab_widget; }

  void update_receive_nodes_tag_list();

  void update_tab_widget();

public Q_SLOTS:
  void on_broadcast_node_updated(const std::string &graph_id, const std::string &tag);

  void on_new_broadcast_tag(const std::string     &tag,
                            const hmap::Terrain   *t_source,
                            const hmap::Heightmap *h_source);

  void on_new_node_created(const std::string &graph_id, const std::string &id);

  void on_node_deleted(const std::string &graph_id, const std::string &id);

  void on_remove_broadcast_tag(const std::string &tag);

Q_SIGNALS:
  void has_been_cleared();

  void has_been_loaded_from_file();

private:
  std::map<std::string, std::shared_ptr<GraphEditor>> graphs;

  std::vector<std::string> graph_order;

  std::map<std::string, BroadcastParam> broadcast_params;

  int id_count = 0;

  bool headless = false;

  // GUI - keep track of the tab widget to allow its update when graphs are added or
  // removed (ownership by app MainWindow)
  QTabWidget *tab_widget = nullptr;

  bool is_dirty;
};

} // namespace hesiod