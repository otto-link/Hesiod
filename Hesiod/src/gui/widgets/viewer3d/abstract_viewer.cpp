/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <typeinfo>

#include <QComboBox>
#include <QGridLayout>

#include "hesiod/model/utils.hpp"
#include "highmap/heightmap.hpp"

#include "hesiod/gui/widgets/abstract_viewer.hpp"
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/open_gl/open_gl_widget.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_node.hpp"

namespace hesiod
{

AbstractViewer::AbstractViewer(GraphNodeWidget *p_graph_node_widget,
                               QWidget         *parent,
                               std::string      label)
    : QWidget(parent), p_graph_node_widget(p_graph_node_widget), label(label)
{
  this->setMinimumSize(DEFAULT_VIEWER_WIDTH, DEFAULT_VIEWER_WIDTH);

  this->connect(p_graph_node_widget,
                &gngui::GraphViewer::node_deleted,
                this,
                &AbstractViewer::on_node_deleted);

  this->connect(p_graph_node_widget,
                &gngui::GraphViewer::node_deselected,
                this,
                &AbstractViewer::on_node_deselected);

  this->connect(p_graph_node_widget,
                &gngui::GraphViewer::node_selected,
                this,
                &AbstractViewer::on_node_selected);

  QGridLayout *layout = new QGridLayout(this);
  this->setLayout(layout);

  // current node preview info
  int row = 0;

  layout->addWidget(new QLabel("Node"), row, 1);
  layout->addWidget(new QLabel("Elevation"), row, 2);
  layout->addWidget(new QLabel("Texture"), row, 3);
  layout->addWidget(new QLabel("Normal map"), row, 4);

  this->button_pin_current_node = new QPushButton("Pin current node");
  this->button_pin_current_node->setCheckable(true);
  this->button_pin_current_node->setChecked(false);

  this->label_node_id = new QLabel();
  this->combo_elev = new QComboBox();
  this->combo_color = new QComboBox();
  this->combo_normal_map = new QComboBox();

  this->connect(this->button_pin_current_node,
                &QPushButton::toggled,
                [this]()
                {
                  // if the button is switch from pinned to unpinned,
                  // reset the view param (and update view param and
                  // both cases)
                  if (!this->button_pin_current_node->isChecked())
                    this->clear();
                });

  this->connect(this->combo_elev,
                QOverload<int>::of(&QComboBox::currentIndexChanged),
                [this]()
                {
                  if (!this->freeze_combo_change_event)
                  {
                    std::string port_id = this->combo_elev->currentText().toStdString();
                    this->current_view_param.port_id_elev = port_id;
                    this->emit_view_param_changed();
                  }
                });

  this->connect(this->combo_color,
                QOverload<int>::of(&QComboBox::currentIndexChanged),
                [this]()
                {
                  if (!this->freeze_combo_change_event)
                  {
                    std::string port_id = this->combo_color->currentText().toStdString();
                    this->current_view_param.port_id_color = port_id;
                    this->emit_view_param_changed();
                  }
                });

  this->connect(
      this->combo_normal_map,
      QOverload<int>::of(&QComboBox::currentIndexChanged),
      [this]()
      {
        if (!this->freeze_combo_change_event)
        {
          std::string port_id = this->combo_normal_map->currentText().toStdString();
          this->current_view_param.port_id_normal_map = port_id;
          this->emit_view_param_changed();
        }
      });

  layout->addWidget(this->button_pin_current_node, row + 1, 0);
  layout->addWidget(this->label_node_id, row + 1, 1);
  layout->addWidget(this->combo_elev, row + 1, 2);
  layout->addWidget(this->combo_color, row + 1, 3);
  layout->addWidget(this->combo_normal_map, row + 1, 4);
}

void AbstractViewer::clear()
{
  this->button_pin_current_node->setChecked(false);
  this->current_node_id = "";
  this->current_view_param = NodeViewParam();
  this->update_view_param_widgets();
  this->emit_view_param_changed();
}

void AbstractViewer::closeEvent(QCloseEvent *event)
{
  Q_EMIT this->widget_close();
  QWidget::closeEvent(event);
}

void AbstractViewer::emit_view_param_changed()
{
  LOG->trace("AbstractViewer::emit_view_param_changed: {}", this->current_node_id);

  BaseNode *p_node = this->p_graph_node_widget->get_p_graph_node()
                         ->get_node_ref_by_id<BaseNode>(this->current_node_id);

  Q_EMIT this->view_param_changed(p_node,
                                  this->current_view_param.port_id_elev,
                                  this->current_view_param.port_id_color,
                                  this->current_view_param.port_id_normal_map);
}

void AbstractViewer::json_from(nlohmann::json const &json)
{
  LOG->trace("AbstractViewer::json_from");

  json_safe_get(json, "label", label);

  json_safe_get(json, "current_node_id", current_node_id);
  json_safe_get(json, "port_id_elev", this->current_view_param.port_id_elev);
  json_safe_get(json, "port_id_color", this->current_view_param.port_id_color);
  json_safe_get(json, "port_id_normal_map", this->current_view_param.port_id_normal_map);

  this->button_pin_current_node->setChecked(json["button_pin_current_node_is_checked"]);

  this->node_view_param_map.clear();

  if (json.contains("node_view_param_map"))
  {
    for (auto &[key, sub_json] : json["node_view_param_map"].items())
    {
      NodeViewParam param;

      json_safe_get(sub_json, "port_id_elev", param.port_id_elev);
      json_safe_get(sub_json, "port_id_color", param.port_id_color);
      json_safe_get(sub_json, "port_id_normal_map", param.port_id_normal_map);

      this->node_view_param_map[key] = param;
    }
  }
  else
  {
    Logger::log()->error("Missing \"node_view_param_map\" in sub_json");
  }
  this->emit_view_param_changed();
}

nlohmann::json AbstractViewer::json_to() const
{
  LOG->trace("AbstractViewer::json_to");

  nlohmann::json json;

  json["label"] = this->label;

  json["current_node_id"] = this->current_node_id;
  json["port_id_elev"] = this->current_view_param.port_id_elev;
  json["port_id_color"] = this->current_view_param.port_id_color;
  json["port_id_normal_map"] = this->current_view_param.port_id_normal_map;

  json["button_pin_current_node_is_checked"] = this->button_pin_current_node->isChecked();

  for (auto &[key, param] : this->node_view_param_map)
  {
    nlohmann::json json_param;
    json_param["port_id_elev"] = param.port_id_elev;
    json_param["port_id_color"] = param.port_id_color;
    json_param["port_id_normal_map"] = param.port_id_normal_map;

    json["node_view_param_map"][key] = json_param;
  }

  LOG->trace("{}", json.dump(4));

  return json;
}

void AbstractViewer::on_node_deleted(const std::string &id)
{
  LOG->trace("AbstractViewer::on_node_deleted {}", id);

  if (id == this->current_node_id)
    this->clear();
}

void AbstractViewer::on_node_deselected(const std::string &id)
{
  LOG->trace("AbstractViewer::on_node_deselected {}", id);

  // prevent any setup change if the current node is pinned
  if (this->button_pin_current_node->isChecked())
    return;

  // bckp view parameter for this specific node (if the viewer is
  // opened while a node is already selected this can leave an empty
  // parameter setup, hence checking if the elevation port is indeed
  // set)
  if (this->current_view_param.port_id_elev != "")
    this->node_view_param_map[id] = this->current_view_param;

  // clear everything
  this->clear();
}

void AbstractViewer::on_node_selected(const std::string &id)
{
  LOG->trace("AbstractViewer::on_node_selected {}", id);

  // prevent any setup change if the current node is pinned
  if (this->button_pin_current_node->isChecked())
    return;

  // if not pinned, keep going
  this->current_node_id = id;

  // retrieve view parameters for this node if it already exists
  if (this->node_view_param_map.contains(id))
  {
    this->current_view_param = this->node_view_param_map.at(id);
  }
  else
  {
    // make an initial of the ports that can be used for the 3D viewer
    BaseNode *p_node = this->p_graph_node_widget->get_p_graph_node()
                           ->get_node_ref_by_id<BaseNode>(id);

    // for the elevation, use first output or first input if there are no output
    {
      int port_idx = 0;
      for (int k = 0; k < p_node->get_nports(); k++)
        if (p_node->get_port_type(k) == gngui::PortType::OUT &&
            p_node->get_data_type(k) == typeid(hmap::Heightmap).name())
        {
          port_idx = k;
          break;
        }

      this->current_view_param.port_id_elev = p_node->get_port_label(port_idx);
    }

    // for the color/texture, use first output of type
    // "HeightmapRGBA", or the last input
    {
      int port_idx = -1;
      for (int k = 0; k < p_node->get_nports(); k++)
        if (p_node->get_port_label(k) == "texture" &&
            p_node->get_data_type(k) == typeid(hmap::HeightmapRGBA).name())
        {
          port_idx = k;
          break;
        }

      if (port_idx >= 0)
        this->current_view_param.port_id_color = p_node->get_port_label(port_idx);
      else
        this->current_view_param.port_id_color = "";
    }

    // for the normal map, use first port named "normal map"
    {
      int port_idx = -1;
      for (int k = 0; k < p_node->get_nports(); k++)
        if (p_node->get_port_label(k) == "normal map" &&
            p_node->get_data_type(k) == typeid(hmap::HeightmapRGBA).name())
        {
          port_idx = k;
          break;
        }

      if (port_idx >= 0)
        this->current_view_param.port_id_normal_map = p_node->get_port_label(port_idx);
      else
        this->current_view_param.port_id_normal_map = "";
    }
  }

  // update the widgets
  this->update_view_param_widgets();
  this->emit_view_param_changed();
}

void AbstractViewer::update_view_param_widgets()
{
  LOG->trace("AbstractViewer::update_view_param_widgets");

  BaseNode *p_node = this->p_graph_node_widget->get_p_graph_node()
                         ->get_node_ref_by_id<BaseNode>(this->current_node_id);

  // used to avoid interfering with the combo events and the data when
  // programmatically modifying the combo
  this->freeze_combo_change_event = true;

  if (p_node)
  {
    // node label/id
    std::string text = p_node->get_caption() + "(" + p_node->get_id() + ")";
    this->label_node_id->setText(text.c_str());

    // boxes
    for (int k = 0; k < p_node->get_nports(); k++)
    {
      std::string text = p_node->get_port_caption(k);
      this->combo_elev->addItem(text.c_str());
      this->combo_color->addItem(text.c_str());
      this->combo_normal_map->addItem(text.c_str());
    }
    this->combo_color->addItem("");
    this->combo_normal_map->addItem("");

    this->combo_elev->setCurrentText(this->current_view_param.port_id_elev.c_str());
    this->combo_color->setCurrentText(this->current_view_param.port_id_color.c_str());
    this->combo_normal_map->setCurrentText(
        this->current_view_param.port_id_normal_map.c_str());
  }
  else
  {
    this->label_node_id->setText("none");
    this->combo_elev->clear();
    this->combo_color->clear();
    this->combo_normal_map->clear();
  }

  this->freeze_combo_change_event = false;
}

} // namespace hesiod
