/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <exception>

#include <QApplication>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QScreen>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "attributes/widgets/abstract_widget.hpp"
#include "attributes/widgets/attributes_widget.hpp"
#include "attributes/widgets/filename_widget.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/gui/widgets/custom_qmenu.hpp"
#include "hesiod/gui/widgets/documentation_popup.hpp"
#include "hesiod/gui/widgets/graph_config_dialog.hpp"
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/gui/widgets/select_string_dialog.hpp"
#include "hesiod/gui/widgets/viewers/viewer_3d.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph/graph_node.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

GraphNodeWidget::GraphNodeWidget(GraphNode *p_graph_node, QWidget *parent)
    : GraphViewer(p_graph_node->get_id(), parent), p_graph_node(p_graph_node)
{
  Logger::log()->trace("GraphNodeWidget::GraphNodeWidget: id: {}", this->get_id());

  // populate node catalog
  this->set_node_inventory(get_node_inventory());
  this->setup_connections();
}

void GraphNodeWidget::add_import_texture_nodes(
    const std::vector<std::string> &texture_paths)
{
  QRectF bbox = this->get_bounding_box();

  AppContext &ctx = HSD_CTX;
  QPointF     delta = QPointF(
      ctx.app_settings.node_editor.position_delta_when_duplicating_node,
      ctx.app_settings.node_editor.position_delta_when_duplicating_node);

  float dy = 0.f;
  for (auto &fname : texture_paths)
  {
    Logger::log()->trace("GraphNodeWidget::add_import_texture_nodes: {}", fname);

    // create both model and graphic nodes
    std::string node_id = this->on_new_node_request("ImportTexture",
                                                    bbox.topRight() +
                                                        QPointF(delta.x(), dy));

    // setup attributes
    BaseNode *p_node = this->p_graph_node->get_node_ref_by_id<BaseNode>(node_id);
    if (p_node)
    {
      // p_node->json_from(json_node["settings"]);
      // p_node->set_id(node_id);
      auto *p_attr = p_node->get_attr_ref()
                         ->at("fname")
                         ->get_ref<attr::FilenameAttribute>();
      p_attr->set_value(fname);

      p_node->compute();
    }
    else
    {
      Logger::log()->error(
          "GraphNodeWidget::add_import_texture_nodes: dangling ptr for node_id {}",
          node_id);
    }

    dy += delta.y();
  }
}

void GraphNodeWidget::automatic_node_layout()
{
  Logger::log()->trace("GraphNodeWidget::automatic_node_layout");

  if (!this->p_graph_node)
    return;

  std::vector<gnode::Point> points = this->p_graph_node->compute_graph_layout_sugiyama();

  AppContext &ctx = HSD_CTX;
  QPointF     delta = QPointF(ctx.app_settings.node_editor.auto_layout_dx,
                          ctx.app_settings.node_editor.auto_layout_dy);
  QRectF      bbox = this->get_bounding_box();
  QPointF     origin = bbox.topLeft();

  size_t k = 0;

  for (auto &[nid, _] : this->p_graph_node->get_nodes())
  {
    if (k > points.size() - 1)
    {
      Logger::log()->error(
          "GraphNodeWidget::automatic_node_layout: computed layout is incoherent "
          "with current graphics node layout");
      return;
    }

    QPointF scene_pos = origin +
                        QPointF(points[k].x * delta.x(), points[k].y * delta.y());
    k++;

    gngui::GraphicsNode *p_gfx_node = this->get_graphics_node_by_id(nid);

    if (p_gfx_node)
      p_gfx_node->setPos(scene_pos);
  }

  QTimer::singleShot(0, this, [this]() { this->zoom_to_content(); });
}

void GraphNodeWidget::clear_all()
{
  this->clear_graphic_scene();
  this->p_graph_node->clear();

  Q_EMIT this->has_been_cleared(this->get_id());
}

void GraphNodeWidget::clear_data_viewers()
{
  for (auto &viewer : this->data_viewers)
  {
    if (Viewer *p_viewer = dynamic_cast<Viewer *>(viewer.get()))
      p_viewer->clear();
  }
  this->data_viewers.clear();
}

void GraphNodeWidget::clear_graphic_scene()
{
  this->set_enabled(false);
  this->clear_data_viewers();
  GraphViewer::clear();
  this->set_enabled(true);
}

void GraphNodeWidget::closeEvent(QCloseEvent *event)
{
  this->clear_data_viewers();
  gngui::GraphViewer::closeEvent(event);
}

bool GraphNodeWidget::get_is_selecting_with_rubber_band() const
{
  return this->is_selecting_with_rubber_band;
}

attr::AttributesWidget *GraphNodeWidget::get_node_attributes_widget(
    const std::string &node_id)
{
  Logger::log()->trace("GraphNodeWidget::get_node_attributes_widget: id {}", node_id);

  BaseNode *p_node = this->p_graph_node->get_node_ref_by_id<BaseNode>(node_id);
  if (!p_node)
    return nullptr;

  // retrieve existing instance if any
  if (this->node_attributes_widget_map.contains(node_id) &&
      this->node_attributes_widget_map.at(node_id))
    return this->node_attributes_widget_map.at(node_id);

  // or generate a widget
  bool        add_save_reset_state_buttons = false;
  std::string window_title = "";

  attr::AttributesWidget *attributes_widget = new attr::AttributesWidget(
      p_node->get_attr_ref(),
      p_node->get_attr_ordered_key_ref(),
      window_title,
      add_save_reset_state_buttons,
      this);

  // change the attribute widget layout spacing a posteriori
  QLayout *retrieved_layout = qobject_cast<QLayout *>(attributes_widget->layout());
  if (retrieved_layout)
  {
    retrieved_layout->setSpacing(4);
    retrieved_layout->setContentsMargins(4, 0, 4, 0);

    for (int i = 0; i < retrieved_layout->count(); ++i)
    {
      QWidget *child = retrieved_layout->itemAt(i)->widget();
      if (!child)
        continue;

      if (auto *inner_layout = child->layout())
      {
        inner_layout->setSpacing(4);
        inner_layout->setContentsMargins(4, 0, 4, 0);
      }
    }
  }

  this->connect(attributes_widget,
                &attr::AttributesWidget::value_changed,
                [this, p_node]()
                {
                  std::string node_id = p_node->get_id();
                  this->p_graph_node->update(node_id);
                });

  this->connect(attributes_widget,
                &attr::AttributesWidget::update_button_released,
                [this, p_node]()
                {
                  std::string node_id = p_node->get_id();
                  this->p_graph_node->update(node_id);
                });

  this->node_attributes_widget_map[node_id] = attributes_widget;

  return attributes_widget;
}

GraphNode *GraphNodeWidget::get_p_graph_node()
{
  if (!this->p_graph_node)
  {
    Logger::log()->critical(
        "GraphNodeWidget::get_p_graph_node: model graph_node reference is a "
        "dangling ptr");
    throw std::runtime_error("dangling ptr");
  }

  return this->p_graph_node;
}

void GraphNodeWidget::json_from(nlohmann::json const &json)
{
  Logger::log()->trace("GraphNodeWidget::json_from");
  this->clear_graphic_scene();

  // to prevent nodes update at each link creation when the loading
  // the graph (very slooow)
  this->update_node_on_connection_finished = false;
  GraphViewer::json_from(json);
  this->update_node_on_connection_finished = true;

  // viewers
  if (json.contains("viewers") && json["viewers"].is_array())
  {
    for (const auto &viewer_json : json["viewers"])
    {
      ViewerType viewer_type = viewer_json["viewer_type"].get<ViewerType>();

      Logger::log()->trace("GraphNodeWidget::json_from: viewer_type: {}",
                           viewer_type_as_string.at(viewer_type));

      // TODO add viewer-type specific handling
      this->on_viewport_request();

      if (auto *p_viewer = dynamic_cast<Viewer3D *>(this->data_viewers.back().get()))
        p_viewer->json_from(viewer_json);
      else
        Logger::log()->error(
            "GraphNodeWidget::json_from: could not retrieve viewer reference");
    }
  }

  // Qt mystery, this needs to be delayed to be effective
  QTimer::singleShot(0, this, [this]() { this->zoom_to_content(); });
}

nlohmann::json GraphNodeWidget::json_import(nlohmann::json const &json, QPointF scene_pos)
{
  // import used when copy/pasting only
  Logger::log()->trace("GraphNodeWidget::json_import");

  // work on a copy of the json to modify the node IDs and return it
  nlohmann::json json_copy = json;

  // nodes
  if (!json_copy["nodes"].is_null())
  {
    // storage of id correspondance storage between original node and it copied version
    std::map<std::string, std::string> copy_id_map = {};

    for (auto &json_node : json_copy["nodes"])
    {
      QPointF pos = scene_pos;
      QPointF delta = QPointF(json_node["scene_position.x"],
                              json_node["scene_position.y"]);

      // create both model and graphic nodes
      std::string node_id = this->on_new_node_request(json_node["caption"], pos + delta);

      // use this new node id and backup it for links
      copy_id_map[json_node["id"].get<std::string>()] = node_id;
      json_node["id"] = node_id;

      // setup attributes
      BaseNode *p_node = this->p_graph_node->get_node_ref_by_id<BaseNode>(node_id);
      p_node->json_from(json_node["settings"]);
      p_node->set_id(node_id);

      this->p_graph_node->update(node_id);
    }

    // links
    if (!json_copy["links"].is_null())
      for (auto &json_link : json_copy["links"])
      {
        std::string node_id_from = json_link["node_out_id"].get<std::string>();
        std::string node_id_to = json_link["node_in_id"].get<std::string>();

        node_id_from = copy_id_map.at(node_id_from);
        node_id_to = copy_id_map.at(node_id_to);

        this->add_link(node_id_from,
                       json_link["port_out_id"],
                       node_id_to,
                       json_link["port_in_id"]);
      }
  }

  return json_copy;
}

nlohmann::json GraphNodeWidget::json_to() const
{
  Logger::log()->trace("GraphNodeWidget::json_to");
  nlohmann::json json = GraphViewer::json_to();

  // add the viewports
  for (auto &sp_widget : this->data_viewers)
    if (auto *p_viewer = dynamic_cast<Viewer *>(sp_widget.get()))
    {
      json["viewers"].push_back(p_viewer->json_to());
    }

  return json;
}

void GraphNodeWidget::on_connection_deleted(const std::string &id_out,
                                            const std::string &port_id_out,
                                            const std::string &id_in,
                                            const std::string &port_id_in,
                                            bool               prevent_graph_update)
{
  Logger::log()->trace("GraphNodeWidget::on_connection_deleted, {}/{} -> {}/{}",
                       id_out,
                       port_id_out,
                       id_in,
                       port_id_in);

  // see GraphNodeWidget::on_node_deleted
  QCoreApplication::processEvents();

  this->p_graph_node->remove_link(id_out, port_id_out, id_in, port_id_in);

  if (!prevent_graph_update)
    this->p_graph_node->update(id_in);
}

void GraphNodeWidget::on_connection_dropped(const std::string &node_id,
                                            const std::string &port_id,
                                            QPointF /*scene_pos*/)
{
  Logger::log()->trace("GraphNodeWidget::on_connection_dropped: {}/{}", node_id, port_id);

  bool ret = this->execute_new_node_context_menu();

  // if a node has indeed being created, arbitrarily connect the first
  // output with the same type has the output from which the link has
  // been emitted
  if (ret)
  {
    Logger::log()->trace("GraphNodeWidget::on_connection_dropped: auto-connecting nodes");

    const std::string node_to = this->last_node_created_id;

    BaseNode *p_node_from = this->p_graph_node->get_node_ref_by_id<BaseNode>(node_id);
    BaseNode *p_node_to = this->p_graph_node->get_node_ref_by_id<BaseNode>(node_to);

    if (p_node_to)
    {
      int         from_port_index = p_node_from->get_port_index(port_id);
      std::string from_type = p_node_from->get_data_type(from_port_index);

      Logger::log()->trace("GraphNodeWidget::on_connection_dropped: from_type: {}",
                           from_type);

      // connect to the first input that has the same type, if any
      for (int k = 0; k < p_node_to->get_nports(); ++k)
      {
        std::string to_type = p_node_to->get_data_type(k);

        if (to_type == from_type)
        {
          std::string port_to_id = p_node_to->get_port_label(k);
          this->add_link(node_id, port_id, node_to, port_to_id);
          break;
        }
      }
    }
    else
    {
      Logger::log()->trace(
          "GraphNodeWidget::on_connection_dropped: p_node_to is nullptr");
    }
  }
}

void GraphNodeWidget::on_connection_finished(const std::string &id_out,
                                             const std::string &port_id_out,
                                             const std::string &id_in,
                                             const std::string &port_id_in)
{
  Logger::log()->trace("GraphNodeWidget::on_connection_finished, {}/{} -> {}/{}",
                       id_out,
                       port_id_out,
                       id_in,
                       port_id_in);

  this->p_graph_node->new_link(id_out, port_id_out, id_in, port_id_in);

  // no update for instance during deserialization to avoid a full
  // graph update at each link creation
  if (this->update_node_on_connection_finished)
    this->p_graph_node->update(id_in);
}

void GraphNodeWidget::on_graph_clear_request()
{
  Logger::log()->trace("GraphNodeWidget::on_graph_clear_request");

  QMessageBox::StandardButton reply = QMessageBox::question(
      nullptr,
      "?",
      "This will clear everything. Are you sure?",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes)
    this->clear_all();
}

void GraphNodeWidget::on_graph_import_request()
{
  Logger::log()->trace("GraphNodeWidget::on_graph_import_request");

  // define import path for dialog fiel
  std::filesystem::path path = this->last_import_path.empty()
                                   ? HSD_CTX.project_model->get_path()
                                   : this->last_import_path;

  this->set_enabled(false);
  QString load_fname = QFileDialog::getOpenFileName(this,
                                                    "Load...",
                                                    path.string().c_str(),
                                                    "Hesiod files (*.hsd)");
  this->set_enabled(true);

  if (!load_fname.isNull() && !load_fname.isEmpty())
  {
    std::string    fname = load_fname.toStdString();
    nlohmann::json json = json_from_file(fname);

    // retrieve the number of graph
    size_t n_graph = json["graph_manager"]["graph_nodes"].size();

    std::vector<std::string> graph_id_list = {};
    for (auto [key, _] : json["graph_manager"]["graph_nodes"].items())
      graph_id_list.push_back(key);

    if (n_graph < 1)
    {
      Logger::log()->warn("GraphNodeWidget::on_graph_import_request: no graph to import");
      return;
    }

    std::string import_graph_id;

    if (n_graph == 1)
    {
      import_graph_id = graph_id_list.back();
    }
    else
    {
      SelectStringDialog dialog(graph_id_list, "Select the graph layer to import:");

      if (dialog.exec() != QDialog::Accepted)
      {
        Logger::log()->trace("GraphNodeWidget::on_graph_import_request: aborted");
        return;
      }

      import_graph_id = dialog.selected_value();
    }

    Logger::log()->trace(
        "GraphNodeWidget::on_graph_import_request: importing {} from file {}",
        import_graph_id,
        fname);

    // build a json file that can be imported, see GraphNodeWidget::json_import
    nlohmann::json json_imp;
    json_imp = json["graph_tabs_widget"]["graph_node_widgets"][import_graph_id];

    // add node settings
    for (auto &json_node : json_imp["nodes"])
    {
      const std::string node_id = json_node["id"];

      // retrieve in the model section the corresponding node and
      // its parameters
      for (auto &json_node_model :
           json["graph_manager"]["graph_nodes"][import_graph_id]["nodes"])
      {
        if (json_node_model["id"] == node_id)
          json_node["settings"] = json_node_model;
      }
    }

    nlohmann::json json_mod = this->json_import(json_imp);

    // set selection on copied nodes
    this->deselect_all();

    for (auto &json_node : json_mod["nodes"])
    {
      const std::string    node_id = json_node["id"].get<std::string>();
      gngui::GraphicsNode *p_gfx_node = this->get_graphics_node_by_id(node_id);

      // Qt mystery, this needs to be delayed to be effective
      QTimer::singleShot(0,
                         this,
                         [p_gfx_node]()
                         {
                           if (p_gfx_node)
                             p_gfx_node->setSelected(true);
                         });
    }

    this->last_import_path = std::filesystem::path(fname).parent_path();
  }
}

void GraphNodeWidget::on_graph_new_request()
{
  Logger::log()->trace("GraphNodeWidget::on_graph_new_request");
  this->clear_all();
}

void GraphNodeWidget::on_graph_reload_request()
{
  Logger::log()->trace("GraphNodeWidget::on_graph_reload_request");

  this->p_graph_node->update();
}

void GraphNodeWidget::on_graph_settings_request()
{
  Logger::log()->trace("GraphNodeWidget::on_graph_settings_request");

  // disabled editor
  this->set_enabled(false);

  // work on a copy of the model configuration before
  // apllying modifications
  GraphConfig       new_model_config = *this->p_graph_node->get_config_ref();
  GraphConfigDialog model_config_editor(&new_model_config);

  int ret = model_config_editor.exec();

  if (ret)
  {
    // backup selected node
    const std::string selected_id = this->get_selected_node_ids().empty()
                                        ? ""
                                        : this->get_selected_node_ids().back();

    // set new config
    *this->p_graph_node->get_config_ref() = new_model_config;

    // backup graphics node reference (to avoid dangling pointer issues in the
    // final loop... because when clearing the model graph, it also destroys the
    // proxy node which is used by the graphics node to keep track of the node id)
    std::map<std::string, gngui::GraphicsNode *> gfx_node_ref_map = {};

    for (auto &[id, _] : this->p_graph_node->get_nodes())
    {
      gfx_node_ref_map[id] = this->get_graphics_node_by_id(id);
    }

    // serialize only the model graph node (not the GUI)
    nlohmann::json json = this->p_graph_node->json_to();

    // clear (only the model part, not the GUI node instances)
    this->p_graph_node->clear();

    // deserialize but do not deserialize config, keep current one
    this->p_graph_node->json_from(json);
    this->p_graph_node->update();

    // update the this proxy node reference in the graphics node
    for (auto &[id, _] : this->p_graph_node->get_nodes())
    {
      gngui::NodeProxy *p_proxy = this->p_graph_node->get_node_ref_by_id<BaseNode>(id)
                                      ->get_proxy_ref();
      gfx_node_ref_map.at(id)->set_p_node_proxy(p_proxy);
      gfx_node_ref_map.at(id)->setSelected(false);
    }

    // set selection back, Qt mystery, this needs to be delayed to be effective
    gngui::GraphicsNode *p_gfx_node = this->get_graphics_node_by_id(selected_id);
    QTimer::singleShot(0,
                       this,
                       [p_gfx_node]()
                       {
                         if (p_gfx_node)
                           p_gfx_node->setSelected(true);
                       });
  }

  this->set_enabled(true);
}

void GraphNodeWidget::on_new_graphics_node_request(const std::string &node_id,
                                                   QPointF            scene_pos)
{
  // GraphicsNodes are not generated by the GraphViewer instance, it
  // is outsourced to the outter nodes manager (this class). This
  // signal emits a request for the creation of a GraphicsNodes
  // (only). This is different from
  // GraphNodeWidget::on_new_node_request which generates both the
  // model and the GUI nodes...

  // This one is actually used for serialization, when the graph
  // viewer requests the creation of a graphics node while the base
  // node has aldready been created when the GraphNode has been
  // deserialized

  Logger::log()->trace("GraphNodeWidget::on_new_graphics_node_request: {} {},{}",
                       node_id,
                       scene_pos.x(),
                       scene_pos.y());

  BaseNode *p_node = this->p_graph_node->get_node_ref_by_id<BaseNode>(node_id);
  this->add_node(p_node->get_proxy_ref(), scene_pos, node_id);
}

std::string GraphNodeWidget::on_new_node_request(const std::string &node_type,
                                                 QPointF            scene_pos)
{
  Logger::log()->trace("GraphNodeWidget::on_new_node_request: node_type {}", node_type);

  if (node_type == "")
    return "";

  // add control node (compute)
  std::string node_id = this->p_graph_node->add_node(node_type);

  // add corresponding graphics node (GUI)
  this->on_new_graphics_node_request(node_id, scene_pos);

  Q_EMIT this->new_node_created(this->get_id(), node_id);

  this->last_node_created_id = node_id;

  return node_id;
}

void GraphNodeWidget::on_node_deleted_request(const std::string &node_id)
{
  Logger::log()->trace("GraphNodeWidget::on_node_deleted_request, node {}", node_id);

  // make sure the Graphics node is destroyed before the Model node to
  // avoid lifetime issues (concerns NodeProxy)
  QCoreApplication::processEvents();

  this->p_graph_node->remove_node(node_id);

  Q_EMIT this->node_deleted(this->get_id(), node_id);
}

void GraphNodeWidget::on_node_pinned(const std::string &node_id, bool state)
{
  Logger::log()->trace("GraphNodeWidget::on_node_pinned, node {}", node_id);

  this->unpin_nodes();

  if (!node_id.empty())
  {
    // TODO make a dedicated GraphViewer method
    BaseNode *p_node = this->p_graph_node->get_node_ref_by_id<BaseNode>(node_id);
    if (!p_node)
      return;

    gngui::GraphicsNode *p_gx_node = this->get_graphics_node_by_id(node_id);
    if (!p_gx_node)
      return;

    p_gx_node->set_is_node_pinned(state);
  }
}

void GraphNodeWidget::on_node_reload_request(const std::string &node_id)
{
  Logger::log()->trace("GraphNodeWidget::on_node_reload_request, node [{}]", node_id);

  this->p_graph_node->update(node_id);
}

void GraphNodeWidget::on_node_right_clicked(const std::string &node_id, QPointF scene_pos)
{
  Logger::log()->trace("GraphNodeWidget::on_node_right_clicked: id {}", node_id);

  BaseNode            *p_node = this->p_graph_node->get_node_ref_by_id<BaseNode>(node_id);
  gngui::GraphicsNode *p_gx_node = this->get_graphics_node_by_id(node_id);

  if (!p_node || !p_gx_node)
    return;

  QPointF item_pos = scene_pos - p_gx_node->scenePos();

  // --- create attribute widget

  // if the click is above the widget, let the widget context menu
  // take the event
  if (item_pos.y() < p_gx_node->get_geometry_ref()->widget_pos.y())
  {

    CustomQMenu *menu = new CustomQMenu();

    attr::AttributesWidget *attributes_widget = this->get_node_attributes_widget(node_id);

    // --- fake ToolBar (no text)

    {
      QWidget     *toolbar = new QWidget;
      QHBoxLayout *layout = new QHBoxLayout(toolbar);
      layout->setContentsMargins(0, 0, 0, 0);

      QToolButton *update_button = new QToolButton;
      update_button->setToolTip("Force update");
      update_button->setIcon(
          update_button->style()->standardIcon(QStyle::SP_BrowserReload));

      QToolButton *bckp_button = new QToolButton;
      bckp_button->setToolTip("Backup state");
      bckp_button->setIcon(bckp_button->style()->standardIcon(QStyle::SP_DriveHDIcon));

      QToolButton *revert_button = new QToolButton;
      revert_button->setToolTip("Revert state");
      revert_button->setIcon(
          revert_button->style()->standardIcon(QStyle::SP_DialogCloseButton));

      QToolButton *load_button = new QToolButton;
      load_button->setToolTip("Load preset");
      load_button->setIcon(
          load_button->style()->standardIcon(QStyle::SP_DialogOpenButton));

      QToolButton *save_button = new QToolButton;
      save_button->setToolTip("Save preset");
      save_button->setIcon(
          save_button->style()->standardIcon(QStyle::SP_DialogSaveButton));

      QToolButton *reset_button = new QToolButton;
      reset_button->setToolTip("Reset settings");
      reset_button->setIcon(
          reset_button->style()->standardIcon(QStyle::SP_MediaSkipBackward));

      QToolButton *help_button = new QToolButton;
      help_button->setToolTip("Help!");
      help_button->setIcon(
          help_button->style()->standardIcon(QStyle::SP_DialogHelpButton));

      for (auto p : {update_button,
                     bckp_button,
                     revert_button,
                     load_button,
                     save_button,
                     reset_button,
                     help_button})
      {
        layout->addWidget(p);
      }

      layout->addStretch(1);

      // connections
      this->connect(update_button,
                    &QPushButton::pressed,
                    [this, p_node]()
                    {
                      std::string node_id = p_node->get_id();
                      this->p_graph_node->update(node_id);
                    });

      this->connect(bckp_button,
                    &QPushButton::pressed,
                    [attributes_widget]() { attributes_widget->on_save_state(); });

      this->connect(revert_button,
                    &QPushButton::pressed,
                    [attributes_widget]()
                    { attributes_widget->on_restore_save_state(); });

      this->connect(load_button,
                    &QPushButton::pressed,
                    [attributes_widget]() { attributes_widget->on_load_preset(); });

      this->connect(save_button,
                    &QPushButton::pressed,
                    [attributes_widget]() { attributes_widget->on_save_preset(); });

      this->connect(reset_button,
                    &QPushButton::pressed,
                    [attributes_widget]()
                    { attributes_widget->on_restore_initial_state(); });

      this->connect(help_button,
                    &QPushButton::pressed,
                    [this, p_node]()
                    {
                      DocumentationPopup *popup = new DocumentationPopup(
                          p_node->get_label(),
                          p_node->get_documentation_html());

                      popup->setAttribute(Qt::WA_DeleteOnClose);
                      popup->show();
                    });

      // convert the widget into a QWidgetAction for the QMenu
      QWidgetAction *widget_action = new QWidgetAction(menu);
      widget_action->setDefaultWidget(toolbar);

      // add the simulated menu bar to the context menu
      menu->addAction(widget_action);
    }

    add_qmenu_spacer(dynamic_cast<QMenu *>(menu), 8);

    // --- add attributes

    // fit attribute settings within a scroll area
    QScrollArea *scroll_area = new QScrollArea(this);
    scroll_area->setWidget(attributes_widget);
    scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // garantee full visibility but within the screen space...
    int max_height = attributes_widget->sizeHint().height();

    QScreen *screen = this->screen();
    if (screen)
    {
      QSize resolution = screen->size();
      max_height = std::min(max_height, static_cast<int>(0.9f * resolution.height()));
    }
    scroll_area->setMinimumHeight(max_height);
    scroll_area->setMinimumWidth(attributes_widget->width() + 16);

    // eventually add action
    QWidgetAction *widget_action = new QWidgetAction(menu);
    widget_action->setDefaultWidget(scroll_area); // attributes_widget);
    menu->addAction(widget_action);

    add_qmenu_spacer(dynamic_cast<QMenu *>(menu), 8);

    // --- show menu

    menu->popup(QCursor::pos());
  }
}

void GraphNodeWidget::on_nodes_copy_request(const std::vector<std::string> &id_list,
                                            const std::vector<QPointF> &scene_pos_list)
{
  Logger::log()->trace("GraphNodeWidget::on_nodes_copy_request");

  // dump the nodes data into the copy buffer (for the node positions,
  // save the node position relative to the mouse cursor)
  this->json_copy_buffer.clear();

  QPoint  mouse_view_pos = this->mapFromGlobal(QCursor::pos());
  QPointF mouse_scene_pos = this->mapToScene(mouse_view_pos);

  // dump to a json with a structure that can be read by
  // GraphViewer::json_from

  json_copy_buffer["nodes"] = nlohmann::json::array();

  for (size_t k = 0; k < id_list.size(); k++)
  {
    nlohmann::json json_node;

    gngui::GraphicsNode *p_gfx_node = this->get_graphics_node_by_id(id_list[k]);
    json_node = p_gfx_node->json_to();

    // override absolute positions with relative positions
    QPointF delta = scene_pos_list[k] - mouse_scene_pos;
    json_node["scene_position.x"] = delta.x();
    json_node["scene_position.y"] = delta.y();

    // add attribute settings to set them back when pasting (not
    // required by GraphViewer::json_from)
    BaseNode *p_node = this->p_graph_node->get_node_ref_by_id<BaseNode>(id_list[k]);
    json_node["settings"] = p_node->json_to();

    this->json_copy_buffer["nodes"].push_back(json_node);
  }

  // backup links between copied nodes
  this->json_copy_buffer["links"] = nlohmann::json::array();

  for (auto &link : this->p_graph_node->get_links())
  {
    // only keep a link if both nodes are in copy buffer
    if (contains(id_list, link.from) && contains(id_list, link.to))
    {
      nlohmann::json json_link;

      gnode::Node *p_from = this->p_graph_node->get_node_ref_by_id(link.from);
      gnode::Node *p_to = this->p_graph_node->get_node_ref_by_id(link.to);

      json_link["node_out_id"] = link.from;
      json_link["node_in_id"] = link.to;
      json_link["port_out_id"] = p_from->get_port_label(link.port_from);
      json_link["port_in_id"] = p_to->get_port_label(link.port_to);

      this->json_copy_buffer["links"].push_back(json_link);
    }
  }

  Q_EMIT this->copy_buffer_has_changed(this->json_copy_buffer);
}

void GraphNodeWidget::on_nodes_duplicate_request(
    const std::vector<std::string> &id_list,
    const std::vector<QPointF>     &scene_pos_list)
{
  Logger::log()->trace("GraphNodeWidget::on_nodes_duplicate_request");

  std::vector<QPointF> scene_pos_shifted = {};

  AppContext &ctx = HSD_CTX;
  QPointF     delta = QPointF(
      ctx.app_settings.node_editor.position_delta_when_duplicating_node,
      ctx.app_settings.node_editor.position_delta_when_duplicating_node);

  for (auto &p : scene_pos_list)
    scene_pos_shifted.push_back(p + delta);

  this->on_nodes_copy_request(id_list, scene_pos_shifted);
  this->on_nodes_paste_request();
}

void GraphNodeWidget::on_nodes_paste_request()
{
  Logger::log()->trace("GraphNodeWidget::on_nodes_paste_request");

  if (!this->json_copy_buffer.is_object())
    return;

  QPointF mouse_scene_pos = this->get_mouse_scene_pos();

  // returned json contains modified node IDs
  nlohmann::json json_mod = this->json_import(this->json_copy_buffer, mouse_scene_pos);

  // set selection on copied nodes
  this->deselect_all();

  for (auto &json_node : json_mod["nodes"])
  {
    const std::string    node_id = json_node["id"].get<std::string>();
    gngui::GraphicsNode *p_gfx_node = this->get_graphics_node_by_id(node_id);
    if (p_gfx_node)
      p_gfx_node->setSelected(true);
  }
}

void GraphNodeWidget::on_viewport_request()
{
  Logger::log()->trace("GraphNodeWidget::on_viewport_request");

  this->data_viewers.push_back(std::make_unique<Viewer3D>(this));
  this->data_viewers.back()->show();

  Viewer *p_viewer = dynamic_cast<Viewer *>(this->data_viewers.back().get());

  // remove the widget from the widget list if it is closed
  this->connect(p_viewer,
                &Viewer::widget_close,
                [this, p_viewer]()
                {
                  std::erase_if(this->data_viewers,
                                [p_viewer](const std::unique_ptr<QWidget> &sptr)
                                { return sptr.get() == p_viewer; });
                });

  // set data of the currently selected node, if any
  std::vector<std::string> selected_ids = this->get_selected_node_ids();

  if (selected_ids.size())
    p_viewer->on_node_selected(selected_ids.back());
}

void GraphNodeWidget::set_json_copy_buffer(nlohmann::json const &new_json_copy_buffer)
{
  this->json_copy_buffer = new_json_copy_buffer;
}

void GraphNodeWidget::setup_connections()
{
  // global actions
  this->connect(this,
                &gngui::GraphViewer::graph_automatic_node_layout_request,
                this,
                &GraphNodeWidget::automatic_node_layout);

  this->connect(this,
                &gngui::GraphViewer::graph_clear_request,
                this,
                &GraphNodeWidget::on_graph_clear_request);

  this->connect(this,
                &gngui::GraphViewer::graph_import_request,
                this,
                &GraphNodeWidget::on_graph_import_request);

  this->connect(this,
                &gngui::GraphViewer::graph_new_request,
                this,
                &GraphNodeWidget::on_graph_new_request);

  this->connect(this,
                &gngui::GraphViewer::graph_reload_request,
                this,
                &GraphNodeWidget::on_graph_reload_request);

  this->connect(this,
                &gngui::GraphViewer::graph_settings_request,
                this,
                &GraphNodeWidget::on_graph_settings_request);

  this->connect(this,
                &gngui::GraphViewer::rubber_band_selection_started,
                [this]() { this->is_selecting_with_rubber_band = true; });

  this->connect(this,
                &gngui::GraphViewer::rubber_band_selection_finished,
                [this]() { this->is_selecting_with_rubber_band = false; });

  // node actions
  this->connect(this,
                &gngui::GraphViewer::connection_deleted,
                this,
                &GraphNodeWidget::on_connection_deleted);

  this->connect(this,
                &gngui::GraphViewer::connection_dropped,
                this,
                &GraphNodeWidget::on_connection_dropped);

  this->connect(this,
                &gngui::GraphViewer::connection_finished,
                this,
                &GraphNodeWidget::on_connection_finished);

  this->connect(this,
                &gngui::GraphViewer::new_graphics_node_request,
                this,
                &GraphNodeWidget::on_new_graphics_node_request);

  this->connect(this,
                &gngui::GraphViewer::new_node_request,
                this,
                &GraphNodeWidget::on_new_node_request);

  this->connect(this,
                &gngui::GraphViewer::node_deleted,
                this,
                &GraphNodeWidget::on_node_deleted_request);

  this->connect(this,
                &gngui::GraphViewer::node_reload_request,
                this,
                &GraphNodeWidget::on_node_reload_request);

  this->connect(this,
                &gngui::GraphViewer::node_right_clicked,
                this,
                &GraphNodeWidget::on_node_right_clicked);

  this->connect(this,
                &gngui::GraphViewer::nodes_copy_request,
                this,
                &GraphNodeWidget::on_nodes_copy_request);

  this->connect(this,
                &gngui::GraphViewer::nodes_duplicate_request,
                this,
                &GraphNodeWidget::on_nodes_duplicate_request);

  this->connect(this,
                &gngui::GraphViewer::nodes_paste_request,
                this,
                &GraphNodeWidget::on_nodes_paste_request);

  // viewers
  this->connect(this,
                &gngui::GraphViewer::viewport_request,
                this,
                &GraphNodeWidget::on_viewport_request);

  this->connect(this->p_graph_node,
                &GraphNode::update_started,
                this,
                &gngui::GraphViewer::on_update_started);

  this->connect(this->p_graph_node,
                &GraphNode::update_finished,
                this,
                &gngui::GraphViewer::on_update_finished);

  // w/ itself
  this->connect(this->p_graph_node,
                &GraphNode::update_started,
                this,
                []() { QApplication::setOverrideCursor(Qt::WaitCursor); });

  this->connect(this->p_graph_node,
                &GraphNode::update_finished,
                this,
                []() { QApplication::restoreOverrideCursor(); });
}

} // namespace hesiod
