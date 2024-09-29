/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <typeinfo>

#include <QComboBox>
#include <QGridLayout>

#include "highmap/heightmap.hpp"

#include "hesiod/gui/widgets/open_gl_render.hpp"
#include "hesiod/gui/widgets/viewer3d.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

Viewer3d::Viewer3d(GraphEditor *p_graph_editor, QWidget *parent, std::string label)
    : QWidget(parent), p_graph_editor(p_graph_editor), label(label)
{
  this->setMinimumSize(512, 512);

  this->connect(p_graph_editor->get_p_viewer(),
                &gngui::GraphViewer::node_deselected,
                this,
                &Viewer3d::on_node_deselected);

  this->connect(p_graph_editor->get_p_viewer(),
                &gngui::GraphViewer::node_selected,
                this,
                &Viewer3d::on_node_selected);

  this->connect(p_graph_editor,
                &GraphEditor::node_compute_finished,
                this,
                &Viewer3d::on_node_compute_finished);

  QGridLayout *layout = new QGridLayout(this);
  this->setLayout(layout);

  // current node preview info
  int row = 0;

  layout->addWidget(new QLabel("Node"), row, 1);
  layout->addWidget(new QLabel("Elevation"), row, 2);
  layout->addWidget(new QLabel("Texture"), row, 3);

  this->button_pin_current_node = new QPushButton("Pin current node");
  this->button_pin_current_node->setCheckable(true);
  this->button_pin_current_node->setChecked(false);

  this->label_node_id = new QLabel();
  this->combo_elev = new QComboBox();
  this->combo_color = new QComboBox();

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

  this->connect(this,
                &Viewer3d::view_param_changed,
                [this]() { LOG->trace("Q_SIGNALS: Viewer3d::view_param_changed"); });

  layout->addWidget(this->button_pin_current_node, row + 1, 0);
  layout->addWidget(this->label_node_id, row + 1, 1);
  layout->addWidget(this->combo_elev, row + 1, 2);
  layout->addWidget(this->combo_color, row + 1, 3);

  row += 2;

  // renderer widget
  {
    this->render_widget = new OpenGLRender();

    this->connect(this,
                  &Viewer3d::view_param_changed,
                  (OpenGLRender *)this->render_widget,
                  &OpenGLRender::set_data);
  }

  layout->addWidget(this->render_widget, row++, 0, 1, 4);

  // update contents
  this->update_view_param_widgets();
  this->emit_view_param_changed();
}

void Viewer3d::json_from(nlohmann::json const &json)
{
  this->label = json["label"];
  this->current_node_id = json["current_node_id"];

  // TODO rebuild map
  this->node_view_param_map.clear();

  for (auto &[key, sub_json] : json["node_view_param_map"].items())
  {
    NodeViewParam param;
    param.port_id_elev = sub_json["port_id_elev"];
    param.port_id_color = sub_json["port_id_color"];

    this->node_view_param_map[key] = param;
  }
}

nlohmann::json Viewer3d::json_to() const
{
  nlohmann::json json;

  json["label"] = this->label;
  json["current_node_id"] = this->current_node_id;

  for (auto &[key, param] : this->node_view_param_map)
  {
    nlohmann::json json_param;
    json_param["port_id_elev"] = param.port_id_elev;
    json_param["port_id_color"] = param.port_id_color;

    json["node_view_param_map"][key] = json_param;
  }

  LOG->trace("{}", json.dump(4));

  return json;
}

void Viewer3d::on_node_compute_finished(const std::string &id)
{
  LOG->trace("Viewer3d::on_node_compute_finished {}", id);

  if (id == this->current_node_id)
    this->emit_view_param_changed();
}

void Viewer3d::on_node_deselected(const std::string &id)
{
  LOG->trace("Viewer3d::on_node_deselected {}", id);

  // prevent any setup change if the current node is pinned
  if (this->button_pin_current_node->isChecked())
    return;

  // if not pinned, keep going
  this->current_node_id = "";

  // backup view parameter for this node
  this->node_view_param_map[id] = this->current_view_param;

  // update the widgets
  this->update_view_param_widgets();
  this->emit_view_param_changed();
}

void Viewer3d::on_node_selected(const std::string &id)
{
  LOG->trace("Viewer3d::on_node_selected {}", id);

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
    BaseNode *p_node = this->p_graph_editor->get_node_ref_by_id<BaseNode>(id);

    // for the elevation, use first output or first input if there are no output
    {
      int port_idx = 0;
      for (int k = 0; k < p_node->get_nports(); k++)
        if (p_node->get_port_type(k) == gngui::PortType::OUT)
        {
          port_idx = k;
          break;
        }

      this->current_view_param.port_id_elev = p_node->get_port_label(port_idx);
    }

    // for the color/texture, use first output of type "HeightMapRGBA"
    {
      int port_idx = -1;
      for (int k = 0; k < p_node->get_nports(); k++)
        if (p_node->get_port_type(k) == gngui::PortType::OUT &&
            p_node->get_data_type(k) == typeid(hmap::HeightMapRGBA).name())
        {
          port_idx = k;
          break;
        }

      if (port_idx >= 0)
        this->current_view_param.port_id_color = p_node->get_port_label(port_idx);
      else
        this->current_view_param.port_id_color = "";
    }
  }

  // update the widgets
  this->update_view_param_widgets();
  this->emit_view_param_changed();
}

void Viewer3d::update_view_param_widgets()
{
  LOG->trace("Viewer3d::update_view_param_widgets");

  BaseNode *p_node = this->p_graph_editor->get_node_ref_by_id<BaseNode>(
      this->current_node_id);

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
    }
    this->combo_color->addItem("");

    this->combo_elev->setCurrentText(this->current_view_param.port_id_elev.c_str());
    this->combo_color->setCurrentText(this->current_view_param.port_id_color.c_str());
  }
  else
  {
    this->label_node_id->setText("none");
    this->combo_elev->clear();
    this->combo_color->clear();
  }

  this->freeze_combo_change_event = false;
}

} // namespace hesiod
