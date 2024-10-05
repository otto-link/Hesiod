/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <filesystem>
#include <fstream>

#include <QApplication>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QWidgetAction>

#include "highmap/geometry/cloud.hpp" // for link colors
#include "highmap/heightmap.hpp"

#include "gnodegui/style.hpp"

#include "attributes/widgets/abstract_widget.hpp"
#include "attributes/widgets/attributes_widget.hpp"

#include "hesiod/graph_editor.hpp"
#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/gui/style.hpp"
#include "hesiod/gui/widgets/model_config_widget.hpp"
#include "hesiod/gui/widgets/viewer3d.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/node_factory.hpp"

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

    this->viewer->set_node_inventory(get_node_inventory());

    // --- connect with graph viewer

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
                  &gngui::GraphViewer::graph_new_request,
                  this,
                  &GraphEditor::on_graph_new_request);

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
                  &gngui::GraphViewer::graph_settings_request,
                  this,
                  &GraphEditor::on_graph_settings_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::new_graphics_node_request,
                  this,
                  &GraphEditor::on_new_graphics_node_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::new_node_request,
                  [this](const std::string &node_type, QPointF scene_pos)
                  { this->on_new_node_request(node_type, scene_pos, nullptr); });

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
                  &gngui::GraphViewer::nodes_copy_request,
                  this,
                  &GraphEditor::on_nodes_copy_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::nodes_duplicate_request,
                  this,
                  &GraphEditor::on_nodes_duplicate_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::nodes_paste_request,
                  this,
                  &GraphEditor::on_nodes_paste_request);

    this->connect(this->viewer.get(),
                  &gngui::GraphViewer::viewport_request,
                  this,
                  &GraphEditor::on_viewport_request);
  }

  // --- syles

  GN_STYLE->node.color_port_data = data_color_map;
  GN_STYLE->node.color_category = category_color_map;
}

void GraphEditor::json_from(nlohmann::json const &json, bool override_config)
{
  GraphNode::json_from(json["graph_node"], override_config);

  std::string version_file = json["Hesiod version"];
  std::string version = "v" + std::to_string(HESIOD_VERSION_MAJOR) + "." +
                        std::to_string(HESIOD_VERSION_MINOR) + "." +
                        std::to_string(HESIOD_VERSION_PATCH);

  if (version != version_file)
    LOG->warn(
        "Hesiod version for this json {} is different for this executable version {}",
        version_file,
        version);

  if (this->viewer && !json["headless"])
    this->viewer->json_from(json["graph_viewer"]);
}

nlohmann::json GraphEditor::json_to() const
{
  nlohmann::json json;

  json["Hesiod version"] = "v" + std::to_string(HESIOD_VERSION_MAJOR) + "." +
                           std::to_string(HESIOD_VERSION_MINOR) + "." +
                           std::to_string(HESIOD_VERSION_PATCH);
  json["graph_node"] = GraphNode::json_to();

  json["headless"] = this->viewer ? false : true;
  if (this->viewer)
    json["graph_viewer"] = this->viewer->json_to();

  return json;
}

void GraphEditor::load_from_file(const std::filesystem::path &load_fname,
                                 bool                         override_config)
{
  // load json
  nlohmann::json json;
  std::ifstream  file(load_fname);

  if (file.is_open())
  {
    file >> json;
    file.close();
    LOG->trace("JSON successfully loaded from {}", load_fname.string());

    this->fname = load_fname;

    this->clear();
    if (this->viewer)
      this->viewer->clear();

    this->json_from(json, override_config);
    this->update();
  }
  else
    LOG->error("Could not open file {} to load JSON", load_fname.string());
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
    this->load_from_file(load_fname.toStdString());
  }
}

void GraphEditor::on_graph_new_request()
{
  LOG->trace("GraphEditor::on_graph_new_request");
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

void GraphEditor::on_graph_settings_request()
{
  LOG->trace("GraphEditor::on_graph_settings_request");

  // work on a copy of the model configuration before
  // apllying modifications
  ModelConfig       new_model_config = *this->get_config_ref();
  ModelConfigWidget model_config_editor(&new_model_config);

  int ret = model_config_editor.exec();

  if (ret)
  {
    *this->get_config_ref() = new_model_config;

    // serialize
    nlohmann::json json = this->json_to();

    // clear (only the model part, not the GUI node instances)
    this->clear();
    if (this->viewer)
      this->viewer->clear();

    // deserialize but do not deserialize config, keep current one
    bool override_config = false;
    this->json_from(json, override_config);

    this->update();
  }
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
                    QApplication::processEvents();
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

void GraphEditor::on_new_node_request(const std::string &node_type,
                                      QPointF            scene_pos,
                                      std::string       *p_new_node_id)
{
  if (node_type == "")
    return;

  // add control node (compute)
  std::string node_id = this->new_node(node_type);

  // add cooresponding graphics node (GUI)
  this->on_new_graphics_node_request(node_id, scene_pos);

  // return node id value if requested
  if (p_new_node_id)
    *p_new_node_id = node_id;
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

void GraphEditor::on_nodes_copy_request(const std::vector<std::string> &id_list,
                                        const std::vector<QPointF>     &scene_pos_list)
{
  LOG->trace("GraphEditor::on_nodes_copy_request");

  // dump the nodes data into the copy buffer (for the node positions,
  // save the node position relative to the mouse cursor)
  this->json_copy_buffer.clear();

  QPoint  mouse_view_pos = this->viewer->mapFromGlobal(QCursor::pos());
  QPointF mouse_scene_pos = this->viewer->mapToScene(mouse_view_pos);

  for (size_t k = 0; k < id_list.size(); k++)
  {
    BaseNode *p_node = this->get_node_ref_by_id<BaseNode>(id_list[k]);

    QPointF delta = scene_pos_list[k] - mouse_scene_pos;

    this->json_copy_buffer[p_node->get_id()] = p_node->json_to();
    this->json_copy_buffer[p_node->get_id()]["delta.x"] = delta.x();
    this->json_copy_buffer[p_node->get_id()]["delta.y"] = delta.y();
  }
}

void GraphEditor::on_nodes_duplicate_request(const std::vector<std::string> &id_list,
                                             const std::vector<QPointF> &scene_pos_list)
{
  LOG->trace("GraphEditor::on_nodes_duplicate_request");

  std::vector<QPointF> scene_pos_shifted = {};

  for (auto &p : scene_pos_list)
    scene_pos_shifted.push_back(p + QPointF(200.f, 200.f));

  this->on_nodes_copy_request(id_list, scene_pos_shifted);
  this->on_nodes_paste_request();
}

void GraphEditor::on_nodes_paste_request()
{
  LOG->trace("GraphEditor::on_nodes_paste_request");

  // add new nodes using the copy buffer data
  QPoint  mouse_view_pos = this->viewer->mapFromGlobal(QCursor::pos());
  QPointF mouse_scene_pos = this->viewer->mapToScene(mouse_view_pos);

  for (auto &[_, json] : this->json_copy_buffer.items())
  {
    std::string node_type = json["label"];
    std::string node_id;
    QPointF     delta = QPointF(json["delta.x"], json["delta.y"]);

    // create the node
    this->on_new_node_request(node_type, mouse_scene_pos + delta, &node_id);

    // retrieve the node state
    BaseNode *p_node = this->get_node_ref_by_id<BaseNode>(node_id);
    p_node->json_from(json);

    // set the ID again because if has been overriden by the deserialization
    p_node->set_id(node_id);

    // recompute
    this->update(node_id);
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
