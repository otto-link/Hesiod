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
#include <QTabWidget>

#include "hesiod/graph_editor.hpp"

namespace hesiod
{

class GraphManager
{
public:
  GraphManager() = default;

  GraphManager(bool headless) : headless(headless) {}

  std::string add_graph_editor(const std::shared_ptr<GraphEditor> &p_graph_editor,
                               const std::string                  &id = "");

  std::string add_graph_editor_after(const std::shared_ptr<GraphEditor> &p_graph_editor,
                                     const std::string &existing_graph_id,
                                     const std::string &id = "");

  std::string add_graph_editor_before(const std::shared_ptr<GraphEditor> &p_graph_editor,
                                      const std::string &existing_graph_id,
                                      const std::string &id = "");

  void dump() const;

  std::filesystem::path get_fname_path() const { return this->fname_path; };

  GraphEditor *get_graph_ref_by_id(const std::string &id);

  GraphEditor *get_graph_ref_by_id_previous(const std::string &id);

  GraphEditor *get_graph_ref_by_id_next(const std::string &id);

  const std::map<std::string, std::shared_ptr<GraphEditor>> &get_graphs()
  {
    return this->graphs;
  }

  bool get_headless() const { return this->headless; }

  bool is_graph_id_available(const std::string &id);

  void json_from(nlohmann::json const &json);

  nlohmann::json json_to() const;

  void load_from_file(const std::string &fname);

  void save_to_file(const std::string &fname) const;

  void set_fname_path(const std::filesystem::path &new_fname_path);

  void set_fname(const std::string &new_fname_path);

  void set_tab_widget(QTabWidget *new_tab_widget) { this->tab_widget = new_tab_widget; }

  void update_tab_widget();

private:
  std::map<std::string, std::shared_ptr<GraphEditor>> graphs;

  std::vector<std::string> graph_order;

  int id_count = 0;

  bool headless = false;

  std::filesystem::path fname_path = "";

  // GUI - keep track of the tab widget to allow its update when graphs are added or
  // removed (ownership by app MainWindow)
  QTabWidget *tab_widget = nullptr;
};

} // namespace hesiod