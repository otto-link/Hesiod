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

// - TODO add clear()
// - TODO add global default config widget
// - TODO handle batch mode
// - TODO import 1 graph
// - TODO multiple node deletion only calls one connect
// - TODO add setting to allow broadcast within the same graph
// - TODO check tags to avoid undue Receive updates
// - TODO remove load/save in graph editor

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

  void dump() const;

  std::filesystem::path get_fname_path() const { return this->fname_path; };

  GraphEditor *get_graph_ref_by_id(const std::string &id);

  GraphEditor *get_graph_ref_by_id_previous(const std::string &id);

  GraphEditor *get_graph_ref_by_id_next(const std::string &id);

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

  void load_from_file(const std::string &fname);

  void remove_graph_editor(const std::string &id);

  void save_to_file(const std::string &fname) const;

  void set_fname_path(const std::filesystem::path &new_fname_path);

  void set_fname(const std::string &new_fname_path);

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

  void on_remove_broadcast_tag(const std::string &tag);

Q_SIGNALS:
  void has_been_loaded_from_file();

private:
  std::map<std::string, std::shared_ptr<GraphEditor>> graphs;

  std::vector<std::string> graph_order;

  std::map<std::string, BroadcastParam> broadcast_params;

  int id_count = 0;

  bool headless = false;

  std::filesystem::path fname_path = "";

  // GUI - keep track of the tab widget to allow its update when graphs are added or
  // removed (ownership by app MainWindow)
  QTabWidget *tab_widget = nullptr;

  bool is_dirty;
};

} // namespace hesiod