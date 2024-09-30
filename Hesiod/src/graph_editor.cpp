/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <filesystem>
#include <fstream>

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QWidgetAction>

#include "gnodegui/style.hpp"

#include "attributes/widgets/abstract_widget.hpp"
#include "attributes/widgets/attributes_widget.hpp"

#include "hesiod/graph_editor.hpp"
#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/gui/widgets/viewer3d.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

GraphEditor::GraphEditor(const std::string           &id,
                         std::shared_ptr<ModelConfig> config,
                         bool                         headless)
    : GraphNode(id, config)
{
  LOG->trace("GraphEditor::GraphEditor, graph id {}", this->get_id());

  if (headless)
  {
    LOG->trace("GraphEditor::GraphEditor, running in headless mode for the graph editor");
  }
  else
  {
    LOG->trace("GraphEditor::GraphEditor, initializing graph GUI");

    // --- instantiate

    this->viewer = std::make_unique<gngui::GraphViewer>(this->get_id());

    this->viewer->set_node_inventory(node_inventory);

    // --- connect with core node graph (GraphNode)

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::connection_deleted,
                  this,
                  &GraphEditor::on_connection_deleted);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::connection_finished,
                  this,
                  &GraphEditor::on_connection_finished);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::graph_clear_request,
                  this,
                  &GraphEditor::on_graph_clear_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::graph_load_request,
                  this,
                  &GraphEditor::on_graph_load_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::graph_reload_request,
                  this,
                  &GraphEditor::on_graph_reload_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::graph_save_as_request,
                  this,
                  &GraphEditor::on_graph_save_as_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::graph_save_request,
                  this,
                  &GraphEditor::on_graph_save_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::new_graphics_node_request,
                  this,
                  &GraphEditor::on_new_graphics_node_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::new_node_request,
                  this,
                  &GraphEditor::on_new_node_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::node_deleted,
                  this,
                  &GraphEditor::on_node_deleted_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::node_reload_request,
                  this,
                  &GraphEditor::on_node_reload_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::node_right_clicked,
                  this,
                  &GraphEditor::on_node_right_clicked);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::viewport_request,
                  this,
                  &GraphEditor::on_viewport_request);
  }

  // --- syles

  // TODO move somewhere else

  // GN_STYLE->node.color_port_data[] = QColor(189, 147, 249, 255);

  GN_STYLE->node.color_category = {{"Converter", QColor(188, 182, 163, 255)},
                                   {"Comment", QColor(170, 170, 170, 255)},
                                   {"Debug", QColor(200, 0, 0, 255)},
                                   {"Math", QColor(0, 43, 54, 255)},
                                   {"Geometry", QColor(101, 123, 131, 255)},
                                   {"Roads", QColor(147, 161, 161, 255)},
                                   {"Routing", QColor(188, 182, 163, 255)},
                                   {"IO", QColor(203, 196, 177, 255)},
                                   {"Features", QColor(181, 137, 0, 255)},
                                   {"Erosion", QColor(203, 75, 22, 255)},
                                   {"Mask", QColor(211, 54, 130, 255)},
                                   {"Filter", QColor(108, 113, 196, 255)},
                                   {"Operator", QColor(108, 113, 196, 255)},
                                   {"Hydrology", QColor(38, 139, 210, 255)},
                                   {"Primitive", QColor(42, 161, 152, 255)},
                                   {"Biomes", QColor(133, 153, 0, 255)}};
}

void GraphEditor::json_from(nlohmann::json const &json)
{
  GraphNode::json_from(json["graph_node"]);

  if (this->viewer && !json["headless"])
  {
    LOG->trace("here");
    this->viewer->json_from(json["graph_viewer"]);
  }
}

nlohmann::json GraphEditor::json_to() const
{
  nlohmann::json json;

  json["graph_node"] = GraphNode::json_to();

  json["headless"] = this->viewer ? false : true;
  if (this->viewer)
    json["graph_viewer"] = this->viewer->json_to();

  return json;
}

void GraphEditor::on_connection_deleted(const std::string &id_out,
                                        const std::string &port_id_out,
                                        const std::string &id_in,
                                        const std::string &port_id_in)
{
  LOG->trace("GraphEditor::on_connection_deleted, {}:{} -> {}:{}",
             id_out,
             port_id_out,
             id_in,
             port_id_in);

  this->remove_link(id_out, port_id_out, id_in, port_id_in);
  this->update(id_in);
}

void GraphEditor::on_connection_finished(const std::string &id_out,
                                         const std::string &port_id_out,
                                         const std::string &id_in,
                                         const std::string &port_id_in)
{
  LOG->trace("GraphEditor::on_connection_finished, {}:{} -> {}:{}",
             id_out,
             port_id_out,
             id_in,
             port_id_in);

  this->new_link(id_out, port_id_out, id_in, port_id_in);
  this->update(id_out);
}

void GraphEditor::on_graph_clear_request()
{
  LOG->trace("GraphEditor::on_graph_clear_request");

  QMessageBox::StandardButton reply = QMessageBox::question(
      nullptr,
      "?",
      "This will clear everything. Are you sure?",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes)
  {
    this->clear();
    if (this->viewer)
      this->viewer->clear();
  }
}

void GraphEditor::on_graph_load_request()
{
  LOG->trace("GraphEditor::on_graph_load_request");

  std::filesystem::path path = this->fname.parent_path();

  QString load_fname = QFileDialog::getOpenFileName(this->viewer.get(),
                                                    "Load...",
                                                    path.string().c_str(),
                                                    "Hesiod files (*.hsd)");

  if (!load_fname.isNull() && !load_fname.isEmpty())
  {
    // load json
    nlohmann::json json;
    std::ifstream  file(load_fname.toStdString());

    if (file.is_open())
    {
      file >> json;
      file.close();
      LOG->trace("JSON successfully loaded from {}", fname.string());

      this->fname = load_fname.toStdString();

      this->clear();
      if (this->viewer)
        this->viewer->clear();

      this->json_from(json);
      this->update();
    }
    else
      LOG->error("Could not open file {} to save JSON", fname.string());
  }
}

void GraphEditor::on_graph_reload_request()
{
  LOG->trace("GraphEditor::on_graph_reload_request");
  this->update();
  // TODO signals back to GUI before / after
}

void GraphEditor::on_graph_save_as_request()
{
  LOG->trace("GraphEditor::on_graph_save_as_request");

  std::filesystem::path path = this->fname.parent_path();

  QString new_fname = QFileDialog::getSaveFileName(this->viewer.get(),
                                                   "Save as...",
                                                   path.string().c_str(),
                                                   "Hesiod files (*.hsd)");

  if (!new_fname.isNull() && !new_fname.isEmpty())
  {
    this->fname = new_fname.toStdString();
    this->on_graph_save_request();
  }
}

void GraphEditor::on_graph_save_request()
{
  LOG->trace("GraphEditor::on_graph_save_request");

  if (this->fname == "")
  {
    this->on_graph_save_as_request();
    return;
  }

  // fill-in json
  nlohmann::json json = this->json_to();

  // save file
  std::ofstream file(fname);

  if (file.is_open())
  {
    file << json.dump(4);
    file.close();
    LOG->trace("JSON successfully written to {}", fname.string());
  }
  else
    LOG->error("Could not open file {} to load JSON", fname.string());
}

void GraphEditor::on_new_graphics_node_request(const std::string &node_id,
                                               QPointF            scene_pos)
{
  // this one is also used for serialization, when the graph viewer
  // requests the creation of a graphics node while the base node has
  // aldready been created when the GraphNode has been deserialized

  if (this->viewer)
  {
    BaseNode *p_node = this->get_node_ref_by_id<BaseNode>(node_id);
    this->viewer->add_node(p_node->get_proxy_ref(), scene_pos, node_id);

    gngui::GraphicsNode *p_gx_node = this->viewer->get_graphics_node_by_id(node_id);

    this->connect(p_node,
                  &BaseNode::compute_started,
                  [this, p_gx_node]()
                  {
                    p_gx_node->on_compute_started();
                    p_gx_node->update();
                  });

    this->connect(p_node,
                  &BaseNode::compute_finished,
                  p_gx_node,
                  &gngui::GraphicsNode::on_compute_finished);

    this->connect(p_node,
                  &BaseNode::compute_finished,
                  [this, p_node]()
                  { Q_EMIT this->node_compute_finished(p_node->get_id()); });
  }
}

void GraphEditor::on_new_node_request(const std::string &node_type, QPointF scene_pos)
{
  if (node_type == "")
    return;

  // add control node (compute)
  std::string node_id = this->new_node(node_type);

  // add cooresponding graphics node (GUI)
  this->on_new_graphics_node_request(node_id, scene_pos);
}

void GraphEditor::on_node_deleted_request(const std::string &node_id)
{
  LOG->trace("GraphNode::on_node_deleted_request, node {}", node_id);

  this->remove_node(node_id);
  if (this->viewer)
    this->viewer->remove_node(node_id);
}

void GraphEditor::on_node_reload_request(const std::string &node_id)
{
  LOG->trace("GraphNode::on_node_reload_request, node {}", node_id);
  this->update(node_id);
  // TODO signals back to GUI before / after
}

void GraphEditor::on_node_right_clicked(const std::string &node_id, QPointF scene_pos)
{
  LOG->trace("GraphNode::on_node_right_clicked, node {}", node_id);

  if (this->viewer)
  {
    BaseNode            *p_node = this->get_node_ref_by_id<BaseNode>(node_id);
    gngui::GraphicsNode *p_gx_node = this->viewer->get_graphics_node_by_id(node_id);

    QPointF item_pos = scene_pos - p_gx_node->scenePos();

    // if the click is above the widget, let the widget context menu
    // take the event
    if (item_pos.y() < p_gx_node->get_geometry_ref()->widget_pos.y())
    {

      QMenu *menu = new QMenu();

      // --- add label

      {
        QLabel *label = new QLabel(p_node->get_caption().c_str());
        resize_font(label, 1);
        QWidgetAction *widget_action = new QWidgetAction(menu);
        widget_action->setDefaultWidget(label);
        menu->addAction(widget_action);
      }

      // menu->addSeparator();

      // --- add attributes

      attr::AttributesWidget *attributes_widget = new attr::AttributesWidget(
          p_node->get_attr_ref(),
          p_node->get_attr_ordered_key_ref());

      {
        QWidgetAction *widget_action = new QWidgetAction(menu);
        widget_action->setDefaultWidget(attributes_widget);
        menu->addAction(widget_action);
      }

      connect(attributes_widget,
              &attr::AttributesWidget::value_changed,
              [this, p_node]()
              {
                std::string node_id = p_node->get_id();
                this->update(node_id);
              });

      connect(attributes_widget,
              &attr::AttributesWidget::update_button_released,
              [this, p_node]() { this->update(p_node->get_id()); });

      menu->popup(QCursor::pos());
    }
  }
}

void GraphEditor::on_viewport_request()
{
  LOG->trace("GraphEditor::on_viewport_request");

  this->viewers.push_back(std::make_unique<Viewer3d>(this));
  this->viewers.back()->show();

  Viewer3d *p_viewer = dynamic_cast<Viewer3d *>(this->viewers.back().get());

  // remove (and hence destroy) the widget from the widget list if it
  // is closed
  this->connect(p_viewer,
                &Viewer3d::widget_close,
                [this, p_viewer]()
                {
                  std::erase_if(this->viewers,
                                [p_viewer](const std::unique_ptr<QWidget> &sptr)
                                { return sptr.get() == p_viewer; });
                });
}

} // namespace hesiod
