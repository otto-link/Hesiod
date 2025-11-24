/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/gui/widgets/viewers/viewer.hpp"
#include "hesiod/gui/widgets/viewers/viewer_3d.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph/graph_node.hpp"

namespace hesiod
{

Viewer::Viewer(QPointer<GraphNodeWidget> p_graph_node_widget_,
               ViewerType                viewer_type_,
               const std::string        &label_,
               QWidget                  *parent)
    : QWidget(parent), p_graph_node_widget(p_graph_node_widget_),
      viewer_type(viewer_type_), label(label_)
{
  Logger::log()->trace("Viewer::Viewer");

  AppContext &ctx = HSD_CTX;

  this->setMinimumSize(ctx.app_settings.viewer.width, ctx.app_settings.viewer.height);
  this->setWindowTitle(this->label.c_str());
}

void Viewer::clear()
{
  this->set_current_node_id("");
  this->view_param = this->get_default_view_param();
  this->is_node_pinned = false;

  this->update_widgets();
  Q_EMIT this->clear_view_request();
}

void Viewer::closeEvent(QCloseEvent *event)
{
  Q_EMIT this->clear();
  Q_EMIT this->widget_close();
  QWidget::closeEvent(event);
}

ViewerNodeParam Viewer::get_default_view_param() const
{
  Logger::log()->critical(
      "Viewer::get_default_view_param: using abstract class method, this "
      "should not be happening");
  return ViewerNodeParam();
}

void Viewer::json_from(nlohmann::json const &json)
{
  Logger::log()->trace("Viewer::json_from");

  // widget geometry
  int x = json["x"];
  int y = json["y"];
  int w = json["width"];
  int h = json["height"];
  this->setGeometry(x, y, w, h);

  // data
  this->label = json["label"];
  this->viewer_type = json["viewer_type"];
  this->is_node_pinned = json["is_node_pinned"];

  this->view_param_map.clear();
  if (json.contains("view_param_map"))
  {
    for (const auto &[key, value] : json["view_param_map"].items())
    {
      ViewerNodeParam param = this->get_default_view_param();
      param.json_from(value);
      this->view_param_map[key] = param;
    }
  }

  this->update_widgets();

  // only set the node selection at the very end, once everything is
  // settled
  this->set_current_node_id(json["current_node_id"]);
}

nlohmann::json Viewer::json_to() const
{
  Logger::log()->trace("Viewer::json_to");

  nlohmann::json json;

  // widget geometry
  QRect geom = this->geometry();
  json["x"] = geom.x();
  json["y"] = geom.y();
  json["width"] = geom.width();
  json["height"] = geom.height();

  // data
  json["label"] = this->label;
  json["viewer_type"] = this->viewer_type;
  json["current_node_id"] = this->current_node_id;
  json["is_node_pinned"] = this->is_node_pinned;

  for (auto &[key, param] : this->view_param_map)
    json["view_param_map"][key] = param.json_to();

  return json;
}

void Viewer::on_node_deleted(const std::string &new_id)
{
  Logger::log()->trace("Viewer::on_node_deleted {}", new_id);

  if (new_id == this->current_node_id)
    this->clear();
}

void Viewer::on_node_deselected(const std::string &new_id)
{
  Logger::log()->trace("Viewer::on_node_deselected {}", new_id);

  // prevent any setup change if the current node is pinned
  if (this->is_node_pinned)
    return;

  // clear everything
  this->clear();
}

void Viewer::on_node_pinned_changed()
{
  Logger::log()->trace("Viewer::on_node_pinned_changed");

  if (!this->p_graph_node_widget)
    return;

  if (this->current_node_id.empty())
  {
    // if there is no selection, don't bother
    // changing the pin button
    this->is_node_pinned = false;
  }
  else
  {
    // if the button is switch from pinned to unpinned,
    // reset the view param (and update view param and
    // both cases)
    if (this->is_node_pinned)
    {
      this->clear();
      this->is_node_pinned = false;

      // at this point, if a node a selected, use it now for the
      // viewport
      if (this->p_graph_node_widget)
      {
        const std::vector<std::string> selected_ids = this->p_graph_node_widget
                                                          ->get_selected_node_ids();

        if (selected_ids.size())
          this->on_node_selected(selected_ids.back());
      }
    }
    else
    {
      this->is_node_pinned = true;
    }
  }

  this->update_widgets();

  Q_EMIT this->node_pinned(this->current_node_id, this->is_node_pinned);
}

void Viewer::on_node_selected(const std::string &new_id)
{
  Logger::log()->trace("Viewer::on_node_selected {}", new_id);

  if (!this->p_graph_node_widget)
    return;

  // prevent any setup change if the current node is pinned
  if (this->is_node_pinned)
    return;

  // do not update selection if the rubber band selection tool is used
  // to avoid multiple useless viewport updates
  if (this->p_graph_node_widget->get_is_selecting_with_rubber_band())
    return;

  this->set_current_node_id(new_id);
}

BaseNode *Viewer::safe_get_node()
{
  if (!this->p_graph_node_widget)
    return nullptr;

  GraphNode *p_graph = this->p_graph_node_widget->get_p_graph_node(); // safe
  if (!p_graph)
    return nullptr;

  BaseNode *p_node = p_graph->get_node_ref_by_id<BaseNode>(this->current_node_id);
  if (!p_node)
    return nullptr;

  return p_node;
}

void Viewer::set_current_node_id(const std::string &new_id)
{
  Logger::log()->trace("Viewer::set_current_node_id: {}", new_id);

  // store current view state
  if (this->current_node_id != "")
    this->view_param_map[this->current_node_id] = this->view_param;

  if (new_id == "" && this->current_node_id != "")
  {
    this->current_node_id = "";
    this->clear();
  }
  else
  {
    this->current_node_id = new_id;

    // get from storage or start anew
    if (this->view_param_map.contains(new_id))
    {
      this->view_param = this->view_param_map.at(new_id);
    }
    else if (this->current_node_id != "")
    {
      // wild guess a default view setup...
      wild_guess_view_param(this->view_param, viewer_type, *this->safe_get_node());
    }
  }

  this->update_widgets();
  Q_EMIT this->current_node_id_changed(this->current_node_id);
}

void Viewer::setup_connections()
{
  Logger::log()->trace("Viewer::setup_connections");

  if (!this->p_graph_node_widget)
    return;

  // close with corresponding GraphNode
  this->connect(this->p_graph_node_widget, &QObject::destroyed, this, &QWidget::close);

  // user actions w/ UI
  this->connect(this->p_graph_node_widget,
                &gngui::GraphViewer::node_deleted,
                this,
                &Viewer::on_node_deleted);

  this->connect(this->p_graph_node_widget,
                &gngui::GraphViewer::node_deselected,
                this,
                &Viewer::on_node_deselected);

  this->connect(this->p_graph_node_widget,
                &gngui::GraphViewer::node_selected,
                this,
                &Viewer::on_node_selected);

  this->connect(this,
                &Viewer::node_pinned,
                this->p_graph_node_widget,
                &GraphNodeWidget::on_node_pinned);

  this->connect(this->button_pin_current_node,
                &QPushButton::clicked,
                this,
                &Viewer::on_node_pinned_changed);

  for (auto &[name, combo] : this->combo_map)
  {
    this->connect(combo,
                  QOverload<int>::of(&QComboBox::currentIndexChanged),
                  [this, combo, name]()
                  {
                    if (!this->prevent_combo_connections)
                    {
                      std::string port_id = combo->currentText().toStdString();
                      this->view_param.port_ids[name] = port_id;
                      this->update_renderer();
                    }
                  });
  }

  // graph update
  this->connect(this->p_graph_node_widget,
                &GraphNodeWidget::compute_finished,
                [this](const std::string &id)
                {
                  if (id == this->current_node_id)
                    this->update_renderer();
                });
}

void Viewer::setup_layout()
{
  Logger::log()->trace("Viewer::setup_layout");

  // create and assign new layout
  QGridLayout *layout = new QGridLayout(this);
  layout->setContentsMargins(2, 0, 2, 0);
  this->setLayout(layout);

  this->button_pin_current_node = new QPushButton("Pin current node");
  this->button_pin_current_node->setCheckable(true);

  layout->addWidget(this->button_pin_current_node, 0, 0);

  int col = 0;
  int row = 1;

  for (auto &[name, _] : view_param.port_ids)
  {
    QLabel    *label = new QLabel(name.c_str());
    QComboBox *combo = new QComboBox();

    layout->addWidget(label, row, col);
    layout->addWidget(combo, row + 1, col);
    col++;

    this->combo_map[name] = combo;
  }
}

void Viewer::update_renderer()
{
  Logger::log()->critical(
      "Viewer::update_renderer: using abstract class method, this should not "
      "be happening");
}

void Viewer::update_widgets()
{
  Logger::log()->trace("Viewer::update_widgets");

  // --- update pinned node button

  if (this->is_node_pinned != this->button_pin_current_node->isChecked())
    this->button_pin_current_node->setChecked(this->is_node_pinned);

  // --- update title

  if (this->current_node_id.empty())
  {
    this->setWindowTitle(this->label.c_str());
  }
  else if (BaseNode *p_node = this->safe_get_node())
  {
    std::string new_title = this->label + " - " + p_node->get_caption() + "(" +
                            p_node->get_id() + ")";
    this->setWindowTitle(new_title.c_str());
  }

  // --- update combo content

  // retriece possible values, corresponding to the node port IDs
  std::vector<std::string> combo_options = {};

  if (!this->current_node_id.empty())
  {
    if (BaseNode *p_node = this->safe_get_node())
    {
      combo_options.reserve(p_node->get_nports());
      for (int k = 0; k < p_node->get_nports(); ++k)
        combo_options.push_back(p_node->get_port_caption(k));
    }
  }

  // update combobox content (prevent render update triggered by the
  // value change to avoid a series of unecessary updates of the
  // renderer and override of the view_param values through combo cpnnections)
  this->prevent_combo_connections = true;

  for (auto &[_, combo] : this->combo_map)
  {
    combo->clear();

    for (auto &option : combo_options)
      combo->addItem(option.c_str());

    combo->addItem("");
    combo->setCurrentIndex(-1);
  }

  // set values
  for (auto &[name, value] : this->view_param.port_ids)
  {
    if (auto it = this->combo_map.find(name); it != this->combo_map.end())
    {
      QComboBox *combo = it->second;
      int        idx = combo->findText(QString::fromStdString(value));

      if (idx >= 0)
        combo->setCurrentIndex(idx);
    }
  }

  this->prevent_combo_connections = false;

  // manually update renderer
  this->update_renderer();
}

} // namespace hesiod
