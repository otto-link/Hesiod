/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <memory>

#include <QWidget>

#include "nlohmann/json.hpp"

#include "qtd/texture_downloader.hpp"

namespace hesiod
{

// forward
class Project;
class GraphManagerWidget;
class GraphTabsWidget;

// =====================================
// ProjectUI
// =====================================
class ProjectUI : public QWidget
{
  Q_OBJECT

public:
  explicit ProjectUI(QWidget *parent = nullptr);

  void load_ui_state(const std::string &fname);
  void save_ui_state(const std::string &fname) const;

  void cleanup();
  void initialize(ProjectModel *project);
  void setup_connections();

  QWidget                *get_widget();
  GraphManagerWidget     *get_graph_manager_widget_ref();
  GraphTabsWidget        *get_graph_tabs_widget_ref();
  qtd::TextureDownloader *get_texture_downloader_ref();

private:
  std::unique_ptr<GraphManagerWidget>     graph_manager_widget;
  std::unique_ptr<GraphTabsWidget>        graph_tabs_widget;
  std::unique_ptr<qtd::TextureDownloader> texture_downloader;
};

} // namespace hesiod