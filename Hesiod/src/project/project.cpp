/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QCoreApplication>

#include "hesiod/logger.hpp"
#include "hesiod/model/graph_manager.hpp"
#include "hesiod/model/utils.hpp"
#include "hesiod/project/project.hpp"

namespace hesiod
{

Project::Project(QObject *parent) : QObject(parent)
{
  Logger::log()->trace("Project::Project");
  this->initialize();
}

void Project::cleanup()
{
  Logger::log()->trace("Project::cleanup");

  QCoreApplication::processEvents();
  this->graph_manager.reset();
  this->path = std::filesystem::path();
  this->name = std::string();
}

GraphManager *Project::get_graph_manager_ref() { return this->graph_manager.get(); }

bool Project::get_is_dirty() const { return this->is_dirty; }

std::string Project::get_name() const { return this->name; }

std::filesystem::path Project::get_path() const { return this->path; }

void Project::initialize()
{
  Logger::log()->trace("Project::initialize");
  this->graph_manager = std::make_unique<GraphManager>();
}

void Project::json_from(nlohmann::json const &json)
{
  Logger::log()->trace("Project::json_from");

  if (!json.contains("graph_manager"))
  {
    Logger::log()->error("Project::json_from: could not parse json for graph_manager");
    return;
  }

  this->graph_manager->json_from(json["graph_manager"]);
  this->graph_manager->update();
}

nlohmann::json Project::json_to() const
{
  Logger::log()->trace("Project::json_to");

  nlohmann::json json;

  json["graph_manager"] = this->graph_manager->json_to();

  return json;
}

void Project::on_has_changed() { this->set_is_dirty(true); }

void Project::set_is_dirty(bool new_state)
{
  if (new_state != this->is_dirty)
  {
    this->is_dirty = new_state;
    Q_EMIT is_dirty_changed();
  }
}

void Project::set_path(const std::filesystem::path &new_path)
{
  this->path = new_path;
  this->name = this->path.stem().string();

  Q_EMIT this->project_name_changed();
}

void Project::set_path(const std::string &new_path)
{
  this->set_path(std::filesystem::path(new_path));
}

void Project::set_name(const std::string &new_name)
{
  this->name = new_name;
  this->path = this->path.parent_path() / this->name / ".hsd";

  Q_EMIT this->project_name_changed();
}

} // namespace hesiod
