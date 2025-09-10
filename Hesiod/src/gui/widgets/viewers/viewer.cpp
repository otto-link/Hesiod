/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>

#include "hesiod/config.hpp"
#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/viewers/viewer.hpp"
#include "hesiod/gui/widgets/viewers/viewer_3d.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_node.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

Viewer::Viewer(GraphNodeWidget   *p_graph_node_widget_,
               ViewerType         viewer_type_,
               const std::string &label_,
               QWidget           *parent)
    : QWidget(parent), p_graph_node_widget(p_graph_node_widget_),
      viewer_type(viewer_type_), label(label_)
{
  LOG->trace("Viewer::Viewer");

  this->setMinimumSize(HSD_CONFIG->viewer.width, HSD_CONFIG->viewer.height);
  this->setWindowTitle(this->label.c_str());
  this->setup_layout();
  this->update_widgets();
  this->setup_connections();
  this->set_viewer_type(this->viewer_type);
}

void Viewer::clear()
{
  this->set_current_node_id("");
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

void Viewer::json_from(nlohmann::json const &json)
{
  // widget geometry
  int x = json["x"];
  int y = json["y"];
  int w = json["width"];
  int h = json["height"];
  this->setGeometry(x, y, w, h);

  // data
  this->label = json["label"];
  this->viewer_type = json["viewer_type"];
  this->set_current_node_id(json["current_node_id"]);
  this->is_node_pinned = json["is_node_pinned"];

  // for (auto &[key, param] : this->node_view_param_map)
  //   param.json_from(json["node_view_param_map"][key]);

  this->update_widgets();
}

nlohmann::json Viewer::json_to() const
{
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

  // for (auto &[key, param] : this->node_view_param_map)
  //   json["node_view_param_map"][key] = param.json_to();

  return json;
}

void Viewer::on_node_deleted(const std::string &new_id)
{
  LOG->trace("Viewer::on_node_deleted {}", new_id);

  if (new_id == this->current_node_id)
    this->clear();
}

void Viewer::on_node_deselected(const std::string &new_id)
{
  LOG->trace("Viewer::on_node_deselected {}", new_id);

  // prevent any setup change if the current node is pinned
  if (this->is_node_pinned)
    return;

  // clear everything
  this->clear();

  LOG->trace("\n{}", this->json_to().dump(4));
}

void Viewer::on_node_selected(const std::string &new_id)
{
  LOG->trace("Viewer::on_node_selected {}", new_id);

  // prevent any setup change if the current node is pinned
  if (this->is_node_pinned)
    return;

  this->set_current_node_id(new_id);
  this->update_widgets();
}

void Viewer::set_current_node_id(const std::string &new_id)
{
  LOG->trace("Viewer::set_current_node_id: {}", new_id);

  this->current_node_id = new_id;

  if (this->current_node_id == "")
  {
    this->setWindowTitle(this->label.c_str());
  }
  else
  {
    // safe getter
    GraphNodeWidget *p_gw = this->p_graph_node_widget;
    if (!p_gw)
    {
      LOG->error(
          "Viewer::set_current_node_id: corresponding graph viewer is dangling ptr");
      this->clear();
      return;
    }

    BaseNode *p_node = p_gw->get_p_graph_node()->get_node_ref_by_id<BaseNode>(
        this->current_node_id);
    if (!p_node)
    {
      LOG->error("Viewer::set_current_node_id: corresponding graph node is dangling ptr");
      this->clear();
      return;
    }

    // update title
    std::string new_title = this->label + " - " + p_node->get_caption() + "(" +
                            p_node->get_id() + ")";
    this->setWindowTitle(new_title.c_str());
  }

  Q_EMIT this->current_node_id_changed(this->current_node_id);
}

void Viewer::set_viewer_type(const ViewerType &new_viewer_type)
{
  LOG->trace("Viewer::set_viewer_type: type = {}",
             viewer_type_as_string.at(new_viewer_type));

  this->viewer_type = new_viewer_type;
  this->setup_layout();
  this->setup_connections(true);
  int row_count = get_row_count(dynamic_cast<QGridLayout *>(this->layout()));
  int col_count = get_column_count(dynamic_cast<QGridLayout *>(this->layout()));

  switch (this->viewer_type)
  {
  case ViewerType::VIEWER3D:
  {
    LOG->trace("Viewer::set_viewer_type: initializing 3D viewer widget...");

    Viewer3D *viewer = new Viewer3D(this->p_graph_node_widget, this);

    this->connect(this,
                  &Viewer::current_node_id_changed,
                  viewer,
                  &Viewer3D::on_current_node_id_changed);

    auto *grid = dynamic_cast<QGridLayout *>(this->layout());
    grid->addWidget(dynamic_cast<QWidget *>(viewer), row_count, 0, 1, col_count);
  }
  break;
  }
}

void Viewer::setup_connections(bool only_widgets)
{
  LOG->trace("Viewer::setup_connections");

  // if the layout is reinitialize, only connect the new widgets
  if (only_widgets)
  {
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
  }

  this->connect(this->button_pin_current_node,
                &QPushButton::clicked,
                [this]()
                {
                  // if the button is switch from pinned to unpinned,
                  // reset the view param (and update view param and
                  // both cases)
                  if (this->is_node_pinned)
                  {
                    this->clear();
                    this->is_node_pinned = false;
                  }
                  else
                  {
                    this->is_node_pinned = true;
                  }

                  this->update_widgets();
                });
}

void Viewer::setup_layout()
{
  LOG->trace("Viewer::setup_layout");

  // remove and delete existing layout if any
  if (QLayout *old_layout = this->layout())
  {
    QLayoutItem *item;
    while ((item = old_layout->takeAt(0)) != nullptr)
    {
      // detach widget (optional, prevents double free)
      if (QWidget *w = item->widget())
        w->setParent(nullptr);
      delete item;
    }
    delete old_layout;
  }

  // create and assign new layout
  QGridLayout *layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(layout);

  this->button_pin_current_node = new QPushButton("Pin current node");
  this->button_pin_current_node->setCheckable(true);

  layout->addWidget(this->button_pin_current_node, 0, 0);
}

void Viewer::update_widgets()
{
  LOG->trace("Viewer::update_widgets");

  this->button_pin_current_node->setChecked(this->is_node_pinned);
}

} // namespace hesiod
