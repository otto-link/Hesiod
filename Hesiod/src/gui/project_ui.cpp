/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QCoreApplication>

#include "hesiod/app/app_settings.hpp"
#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/gui/project_ui.hpp"
#include "hesiod/gui/widgets/graph_manager_widget.hpp"
#include "hesiod/gui/widgets/graph_tabs_widget.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/utils.hpp"
#include "hesiod/project/project.hpp"

namespace hesiod
{

ProjectUI::ProjectUI(QWidget *parent) : QWidget(parent)
{
  Logger::log()->trace("Project::Project");
}

void ProjectUI::cleanup()
{
  Logger::log()->trace("ProjectUI::cleanup");

  QCoreApplication::processEvents();
  this->graph_manager_widget.reset();
  this->graph_tabs_widget.reset();
  this->texture_downloader.reset();
}

GraphManagerWidget *ProjectUI::get_graph_manager_widget_ref()
{
  return this->graph_manager_widget.get();
}

GraphTabsWidget *ProjectUI::get_graph_tabs_widget_ref()
{
  return this->graph_tabs_widget.get();
}

qtd::TextureDownloader *ProjectUI::get_texture_downloader_ref()
{
  return this->texture_downloader.get();
}

QWidget *ProjectUI::get_widget()
{
  return static_cast<QWidget *>(this->graph_tabs_widget.get());
}

void ProjectUI::load_ui_state(const std::string &fname)
{
  Logger::log()->trace("ProjectUI::load: {}", fname);

  if (!std::filesystem::exists(std::filesystem::path(fname)))
  {
    Logger::log()->error("ProjectUI::load: file does not exist: {}", fname);
    return;
  }

  nlohmann::json json = json_from_file(fname);

  if (!json.contains("graph_manager_widget") || !json.contains("graph_tabs_widget"))
  {
    Logger::log()->error("Project::json_from: could not parse json");
    return;
  }

  this->graph_manager_widget->json_from(json["graph_manager_widget"]);
  this->graph_tabs_widget->json_from(json["graph_tabs_widget"]);
}

void ProjectUI::initialize(Project *project)
{
  Logger::log()->trace("Project::initialize");

  // safeguards...
  if (!project)
    Logger::log()->error("Project::initialize: project ptr is NULL");

  GraphManager *p_graph_manager = project->get_graph_manager_ref();

  if (!p_graph_manager)
    Logger::log()->error("Project::initialize: project graph_manager ptr is NULL");

  // initialize UI components (only tabs assign to the current parent
  // widget, so that the graph manager and the texture downloader will
  // show as floating windows)
  this->graph_manager_widget = std::make_unique<GraphManagerWidget>(p_graph_manager);
  this->graph_tabs_widget = std::make_unique<GraphTabsWidget>(p_graph_manager, this);

  this->graph_tabs_widget->set_show_node_settings_widget(
      HSD_CTX.app_settings.node_editor.show_node_settings_pan);

  if (HSD_CTX.app_settings.global.enable_texture_downloader)
    this->texture_downloader = std::make_unique<qtd::TextureDownloader>();

  // connections
  this->setup_connections();
}

void ProjectUI::save_ui_state(const std::string &fname) const
{
  Logger::log()->trace("ProjectUI::save: {}", fname);

  nlohmann::json json;
  json["graph_manager_widget"] = this->graph_manager_widget->json_to();
  json["graph_tabs_widget"] = this->graph_tabs_widget->json_to();
  json_to_file(json, fname, /* merge_with_existing_content */ true);
}

void ProjectUI::setup_connections()
{
  Logger::log()->trace("Project::setup_connections");

  // GraphManagerWidget -> GraphTabsWidget
  this->connect(this->graph_manager_widget.get(),
                &GraphManagerWidget::graph_removed,
                this->graph_tabs_widget.get(),
                &GraphTabsWidget::update_tab_widget);

  this->connect(this->graph_manager_widget.get(),
                &GraphManagerWidget::list_reordered,
                this->graph_tabs_widget.get(),
                &GraphTabsWidget::update_tab_widget);

  this->connect(this->graph_manager_widget.get(),
                &GraphManagerWidget::new_graph_added,
                this->graph_tabs_widget.get(),
                &GraphTabsWidget::update_tab_widget);

  this->connect(this->graph_manager_widget.get(),
                &GraphManagerWidget::selected_graph_changed,
                this->graph_tabs_widget.get(),
                &GraphTabsWidget::set_selected_tab);

  // GraphTabsWidget -> GraphManagerWidget
  this->connect(this->graph_tabs_widget.get(),
                &GraphTabsWidget::has_been_cleared,
                this->graph_manager_widget.get(),
                &GraphManagerWidget::update_combobox);

  this->connect(this->graph_tabs_widget.get(),
                &GraphTabsWidget::new_node_created,
                this,
                [this](const std::string &graph_id, const std::string & /* id */)
                { this->graph_manager_widget->update_combobox(graph_id); });

  this->connect(this->graph_tabs_widget.get(),
                &GraphTabsWidget::node_deleted,
                this,
                [this](const std::string &graph_id, const std::string & /* id */)
                { this->graph_manager_widget->update_combobox(graph_id); });

  // qtd::TextureDownloader -> GraphTabsWidget
  this->connect(this->texture_downloader.get(),
                &qtd::TextureDownloader::textures_retrieved,
                this->graph_tabs_widget.get(),
                &GraphTabsWidget::on_textures_request);
}

} // namespace hesiod
