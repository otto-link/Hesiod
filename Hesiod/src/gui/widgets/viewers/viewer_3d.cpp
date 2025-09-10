/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>

#include "hesiod/config.hpp"
#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/viewers/viewer_3d.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_node.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

Viewer3D::Viewer3D(GraphNodeWidget *p_graph_node_widget_, QWidget *parent)
    : QWidget(parent), p_graph_node_widget(p_graph_node_widget_)
{
  LOG->trace("Viewer3D::Viewer3D");

  this->setup_layout();
  this->update_combos();
  this->setup_connections();
}

void Viewer3D::clear()
{
  LOG->trace("Viewer3D::clear");

  this->current_node_id = "";
  this->view_param = Viewer3DNodeParam();
  this->terrain_renderer->clear();

  // prevent render update triggered by the combo update
  this->prevent_renderer_update = true;
  for (auto &[_, combo] : this->combo_map)
  {
    combo->clear();
    combo->setCurrentIndex(-1);
  }
  this->prevent_renderer_update = false;

  this->update_renderer();

  this->update();
}

void Viewer3D::json_from(nlohmann::json const &json)
{
  for (auto &[key, param] : this->view_param_map)
    param.json_from(json["view_param_map"][key]);

  this->on_current_node_id_changed(json["current_node_id"]);
}

nlohmann::json Viewer3D::json_to() const
{
  nlohmann::json json;

  json["current_node_id"] = this->current_node_id;

  for (auto &[key, param] : this->view_param_map)
    json["view_param_map"][key] = param.json_to();

  return json;
}

void Viewer3D::on_current_node_id_changed(const std::string &new_id)
{
  LOG->trace("Viewer3D::on_current_node_id_changed: {}", new_id);

  if (new_id == "")
  {
    // store current view state
    if (this->current_node_id != "")
      this->view_param_map[this->current_node_id] = this->view_param;

    this->current_node_id = new_id;
    this->clear();
  }
  else
  {
    this->current_node_id = new_id;

    // get from storage or start anew
    if (this->view_param_map.contains(this->current_node_id))
    {
      this->view_param = this->view_param_map.at(this->current_node_id);
    }
    else
    {
      // wild guess a default view setup...
    }
  }

  this->update_combos();
}

void Viewer3D::setup_connections()
{
  LOG->trace("Viewer3D::setup_connections");

  this->connect(this->p_graph_node_widget->get_p_graph_node(),
                &GraphNode::compute_finished,
                [this](const std::string &graph_id, const std::string &id)
                {
                  if (id == this->current_node_id)
                    this->update_renderer();
                });

  for (auto &[name, combo] : this->combo_map)
  {
    this->connect(combo,
                  QOverload<int>::of(&QComboBox::currentIndexChanged),
                  [this, combo, name]()
                  {
                    std::string port_id = combo->currentText().toStdString();
                    this->view_param.port_ids[name] = port_id;

                    if (!this->prevent_renderer_update)
                      this->update_renderer();
                  });
  }
}

void Viewer3D::setup_layout()
{
  LOG->trace("Viewer3D::setup_layout");

  QGridLayout *layout = new QGridLayout(this);
  this->setLayout(layout);

  int col = 0;
  int row = 0;

  for (auto &[name, _] : view_param.port_ids)
  {
    QLabel    *label = new QLabel(name.c_str());
    QComboBox *combo = new QComboBox();

    layout->addWidget(label, row, col);
    layout->addWidget(combo, row + 1, col);
    col++;

    this->combo_map[name] = combo;
  }

  row += 2;

  this->terrain_renderer = new qtr::RenderWidget("", this);
  layout->addWidget(this->terrain_renderer, row, 0, 1, col);
}

void Viewer3D::update_combos()
{

  LOG->trace("Viewer3D::update_combos");

  // --- update combo content

  // retriece possible values, corresponding to the node port IDs
  std::vector<std::string> combo_options = {};

  if (this->current_node_id != "" && this->p_graph_node_widget)
  {
    // safe getter
    GraphNodeWidget *p_gw = this->p_graph_node_widget;
    if (!p_gw)
    {
      LOG->error("Viewer::update_combos: corresponding graph viewer is dangling ptr");
      this->clear();
      return;
    }

    BaseNode *p_node = p_gw->get_p_graph_node()->get_node_ref_by_id<BaseNode>(
        this->current_node_id);
    if (!p_node)
    {
      LOG->error("Viewer::update_combos: corresponding graph node is dangling ptr");
      this->clear();
      return;
    }

    // retrieve node port infos
    for (int k = 0; k < p_node->get_nports(); k++)
    {
      std::string port_caption = p_node->get_port_caption(k);
      combo_options.push_back(port_caption);
    }
  }

  // update combobox content (prevent render update triggered by the
  // value change to avoid a series of unecessary updates of the
  // renderer)
  this->prevent_renderer_update = true;

  for (auto &[_, combo] : this->combo_map)
  {
    for (auto &option : combo_options)
      combo->addItem(option.c_str());
    combo->addItem("");
  }

  // set value
  for (auto &[name, value] : view_param.port_ids)
    this->combo_map[name]->setCurrentText(value.c_str());

  this->prevent_renderer_update = false;

  // manually update renderer
  this->update_renderer();
}

void Viewer3D::update_renderer()
{
  // TODO update
  LOG->debug("Viewer3D::setup_connections: need update");
}

} // namespace hesiod
