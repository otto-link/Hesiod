/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <filesystem>
#include <string>

#include <QObject>

#include "nlohmann/json.hpp"

namespace hesiod
{

// forward
class GraphManager;

// =====================================
// Project
// =====================================
class Project : public QObject
{
  Q_OBJECT
public:
  explicit Project(QObject *parent = nullptr);

  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

  void cleanup();
  void initialize();

  GraphManager         *get_graph_manager_ref();
  bool                  get_is_dirty() const;
  std::string           get_name() const;
  std::filesystem::path get_path() const;
  void                  set_is_dirty(bool new_state);
  void                  set_path(const std::filesystem::path &new_path);
  void                  set_path(const std::string &new_path);
  void                  set_name(const std::string &new_name);

signals:
  void project_name_changed();
  void is_dirty_changed();

public slots:
  void on_has_changed();

private:
  // --- Members
  std::string                   name;
  std::filesystem::path         path;
  std::unique_ptr<GraphManager> graph_manager;
  bool                          is_dirty = false;
};

} // namespace hesiod