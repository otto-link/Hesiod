/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "attributes/widgets/abstract_widget.hpp"
#include "attributes/widgets/attributes_widget.hpp"

#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/gui/style.hpp"
#include "hesiod/gui/widgets/graph_node_widget.hpp"
#include "hesiod/gui/widgets/model_config_widget.hpp"
#include "hesiod/gui/widgets/viewer3d.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_node.hpp"
#include "hesiod/model/nodes/node_factory.hpp"

namespace hesiod
{

GraphNodeWidget::GraphNodeWidget(GraphNode *p_graph_node)
    : GraphViewer(p_graph_node->get_id()), p_graph_node(p_graph_node)
{
  LOG->trace("GraphNodeWidget::GraphNodeWidget: id: {}", this->get_id());

  // populate node catalog
  this->set_node_inventory(get_node_inventory());

  this->setup_connections();
}

void GraphNodeWidget::clear_all()
{
  this->clear_graphic_scene();
  this->p_graph_node->clear();

  Q_EMIT this->has_been_cleared(this->get_id());
}

void GraphNodeWidget::clear_data_viewers()
{
  for (auto &v : this->data_viewers)
  {
    AbstractViewer *p_viewer = dynamic_cast<AbstractViewer *>(v.get());
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

GraphNode *GraphNodeWidget::get_p_graph_node() { return this->p_graph_node; }

void GraphNodeWidget::json_from(nlohmann::json const &json)
{
  LOG->trace("GraphNodeWidget::json_from");

  this->clear_graphic_scene();

  // to prevent nodes update at each link creation when the loading
  // the graph (very slooow)
  this->update_node_on_connection_finished = false;
  GraphViewer::json_from(json);
  this->update_node_on_connection_finished = true;
}

nlohmann::json GraphNodeWidget::json_to() const
{
  LOG->trace("GraphNodeWidget::json_to");
  return GraphViewer::json_to();
}

void GraphNodeWidget::on_connection_deleted(const std::string &id_out,
                                            const std::string &port_id_out,
                                            const std::string &id_in,
                                            const std::string &port_id_in)
{
  LOG->trace("GraphNodeWidget::on_connection_deleted, {}/{} -> {}/{}",
             id_out,
             port_id_out,
             id_in,
             port_id_in);

  this->p_graph_node->remove_link(id_out, port_id_out, id_in, port_id_in);
  this->p_graph_node->update(id_in);
}

void GraphNodeWidget::on_connection_finished(const std::string &id_out,
                                             const std::string &port_id_out,
                                             const std::string &id_in,
                                             const std::string &port_id_in)
{
  LOG->trace("GraphNodeWidget::on_connection_finished, {}/{} -> {}/{}",
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
  LOG->trace("GraphNodeWidget::on_graph_clear_request");

  QMessageBox::StandardButton reply = QMessageBox::question(
      nullptr,
      "?",
      "This will clear everything. Are you sure?",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes)
    this->clear_all();
}

void GraphNodeWidget::on_graph_new_request()
{
  LOG->trace("GraphNodeWidget::on_graph_new_request");
  this->clear_all();
}

void GraphNodeWidget::on_graph_reload_request()
{
  LOG->trace("GraphNodeWidget::on_graph_reload_request");

  this->p_graph_node->update();
}

void GraphNodeWidget::on_graph_settings_request()
{
  LOG->trace("GraphNodeWidget::on_graph_settings_request");

  // disabled editor
  this->set_enabled(false);

  // work on a copy of the model configuration before
  // apllying modifications
  ModelConfig       new_model_config = *this->p_graph_node->get_config_ref();
  ModelConfigWidget model_config_editor(&new_model_config);

  int ret = model_config_editor.exec();

  if (ret)
  {
    *this->p_graph_node->get_config_ref() = new_model_config;

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

      this->get_graphics_node_by_id(id)->set_p_node_proxy(p_proxy);
    }
  }

  // deactivate drag to fix some event issue between the dialog box and the graphics view
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

  LOG->trace("GraphNodeWidget::on_new_graphics_node_request: {} {},{}",
             node_id,
             scene_pos.x(),
             scene_pos.y());

  BaseNode *p_node = this->p_graph_node->get_node_ref_by_id<BaseNode>(node_id);
  this->add_node(p_node->get_proxy_ref(), scene_pos, node_id);
}

std::string GraphNodeWidget::on_new_node_request(const std::string &node_type,
                                                 QPointF            scene_pos)
{
  LOG->trace("GraphNodeWidget::on_new_node_request: node_type {}", node_type);

  if (node_type == "")
    return "";

  // add control node (compute)
  std::string node_id = this->p_graph_node->add_node(node_type);

  // add corresponding graphics node (GUI)
  this->on_new_graphics_node_request(node_id, scene_pos);

  Q_EMIT this->new_node_created(this->get_id(), node_id);

  return node_id;
}

void GraphNodeWidget::on_node_deleted_request(const std::string &node_id)
{
  LOG->trace("GraphNodeWidget::on_node_deleted_request, node {}", node_id);

  this->p_graph_node->remove_node(node_id);

  Q_EMIT this->node_deleted(this->get_id(), node_id);
}

void GraphNodeWidget::on_node_reload_request(const std::string &node_id)
{
  LOG->trace("GraphNodeWidget::on_node_reload_request, node [{}]", node_id);

  this->p_graph_node->update(node_id);
}

void GraphNodeWidget::on_node_right_clicked(const std::string &node_id, QPointF scene_pos)
{
  LOG->trace("GraphNodeWidget::on_node_right_clicked: id {}", node_id);

  BaseNode            *p_node = this->p_graph_node->get_node_ref_by_id<BaseNode>(node_id);
  gngui::GraphicsNode *p_gx_node = this->get_graphics_node_by_id(node_id);

  QPointF item_pos = scene_pos - p_gx_node->scenePos();

  // if the click is above the widget, let the widget context menu
  // take the event
  if (item_pos.y() < p_gx_node->get_geometry_ref()->widget_pos.y())
  {

    QMenu *menu = new QMenu();

    // create the widget holding all the attribute widgets (created
    // here, needed for connect below)
    bool        add_save_reset_state_buttons = false;
    std::string window_title = "";

    attr::AttributesWidget *attributes_widget = new attr::AttributesWidget(
        p_node->get_attr_ref(),
        p_node->get_attr_ordered_key_ref(),
        window_title,
        add_save_reset_state_buttons);

    // --- add label

    {
      QLabel *label = new QLabel(p_node->get_caption().c_str());
      resize_font(label, 2);
      QWidgetAction *widget_action = new QWidgetAction(menu);
      widget_action->setDefaultWidget(label);
      menu->addAction(widget_action);
    }

    // --- fake ToolBar

    {
      QWidget     *toolbar = new QWidget;
      QHBoxLayout *layout = new QHBoxLayout(toolbar);
      layout->setContentsMargins(0, 0, 0, 0);

      QToolButton *update_button = new QToolButton;
      update_button->setText("Force\nupdate");
      update_button->setIcon(
          update_button->style()->standardIcon(QStyle::SP_BrowserReload));

      QToolButton *bckp_button = new QToolButton;
      bckp_button->setText("Backup\nstate");
      bckp_button->setIcon(bckp_button->style()->standardIcon(QStyle::SP_DriveHDIcon));

      QToolButton *reset_button = new QToolButton;
      reset_button->setText("Revert\nstate");
      reset_button->setIcon(
          reset_button->style()->standardIcon(QStyle::SP_DialogCloseButton));

      QToolButton *load_button = new QToolButton;
      load_button->setText("Load\npreset");
      load_button->setIcon(
          load_button->style()->standardIcon(QStyle::SP_DialogOpenButton));

      QToolButton *save_button = new QToolButton;
      save_button->setText("Save\npreset");
      save_button->setIcon(
          save_button->style()->standardIcon(QStyle::SP_DialogSaveButton));

      QToolButton *help_button = new QToolButton;
      help_button->setText("Help!");
      help_button->setIcon(
          help_button->style()->standardIcon(QStyle::SP_DialogHelpButton));

      for (auto p : {update_button,
                     bckp_button,
                     reset_button,
                     load_button,
                     save_button,
                     help_button})
      {
        p->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        resize_font(p, -1);
        p->setFixedSize(48, 64);
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

      this->connect(reset_button,
                    &QPushButton::pressed,
                    [attributes_widget]()
                    { attributes_widget->on_restore_save_state(); });

      this->connect(load_button,
                    &QPushButton::pressed,
                    [attributes_widget]() { attributes_widget->on_load_preset(); });

      this->connect(save_button,
                    &QPushButton::pressed,
                    [attributes_widget]() { attributes_widget->on_save_preset(); });

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

    // --- add attributes

    // change the attribute widget layout spacing a posteriori
    QVBoxLayout *retrieved_layout = qobject_cast<QVBoxLayout *>(
        attributes_widget->layout());
    if (retrieved_layout)
    {
      retrieved_layout->setSpacing(0);
      retrieved_layout->setContentsMargins(0, 0, 0, 0);
    }

    QWidgetAction *widget_action = new QWidgetAction(menu);
    widget_action->setDefaultWidget(attributes_widget);
    menu->addAction(widget_action);

    connect(attributes_widget,
            &attr::AttributesWidget::value_changed,
            [this, p_node]()
            {
              std::string node_id = p_node->get_id();
              this->p_graph_node->update(node_id);
            });

    connect(attributes_widget,
            &attr::AttributesWidget::update_button_released,
            [this, p_node]()
            {
              std::string node_id = p_node->get_id();
              this->p_graph_node->update(node_id);
            });

    // --- show menu

    menu->popup(QCursor::pos());
  }
}

void GraphNodeWidget::on_nodes_copy_request(const std::vector<std::string> &id_list,
                                            const std::vector<QPointF> &scene_pos_list)
{
  LOG->trace("GraphNodeWidget::on_nodes_copy_request");

  // dump the nodes data into the copy buffer (for the node positions,
  // save the node position relative to the mouse cursor)
  this->json_copy_buffer.clear();

  QPoint  mouse_view_pos = this->mapFromGlobal(QCursor::pos());
  QPointF mouse_scene_pos = this->mapToScene(mouse_view_pos);

  for (size_t k = 0; k < id_list.size(); k++)
  {
    BaseNode *p_node = this->p_graph_node->get_node_ref_by_id<BaseNode>(id_list[k]);

    QPointF delta = scene_pos_list[k] - mouse_scene_pos;

    this->json_copy_buffer[p_node->get_id()] = p_node->json_to();
    this->json_copy_buffer[p_node->get_id()]["delta.x"] = delta.x();
    this->json_copy_buffer[p_node->get_id()]["delta.y"] = delta.y();
  }
}

void GraphNodeWidget::on_nodes_duplicate_request(
    const std::vector<std::string> &id_list,
    const std::vector<QPointF>     &scene_pos_list)
{
  LOG->trace("GraphNodeWidget::on_nodes_duplicate_request");

  std::vector<QPointF> scene_pos_shifted = {};

  for (auto &p : scene_pos_list)
    scene_pos_shifted.push_back(p + QPointF(200.f, 200.f));

  this->on_nodes_copy_request(id_list, scene_pos_shifted);
  this->on_nodes_paste_request();
}

void GraphNodeWidget::on_nodes_paste_request()
{
  LOG->trace("GraphNodeWidget::on_nodes_paste_request");

  // add new nodes using the copy buffer data
  QPoint  mouse_view_pos = this->mapFromGlobal(QCursor::pos());
  QPointF mouse_scene_pos = this->mapToScene(mouse_view_pos);

  for (auto &[_, json] : this->json_copy_buffer.items())
  {
    std::string node_type = json["label"];
    QPointF     delta = QPointF(json["delta.x"], json["delta.y"]);

    // create the node (node_id is output)
    std::string node_id = this->on_new_node_request(node_type, mouse_scene_pos + delta);

    // retrieve the node state
    BaseNode *p_node = this->p_graph_node->get_node_ref_by_id<BaseNode>(node_id);
    p_node->json_from(json);

    // set the ID again because if has been overriden by the deserialization
    p_node->set_id(node_id);

    // recompute
    this->p_graph_node->update(node_id);

    Q_EMIT this->new_node_created(this->get_id(), node_id);
  }
}

void GraphNodeWidget::on_viewport_request()
{
  LOG->trace("GraphNodeWidget::on_viewport_request");

  this->data_viewers.push_back(std::make_unique<Viewer3d>(this));
  this->data_viewers.back()->show();

  Viewer3d *p_viewer = dynamic_cast<Viewer3d *>(this->data_viewers.back().get());

  // remove the widget from the widget list if it is closed
  this->connect(p_viewer,
                &Viewer3d::widget_close,
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

void GraphNodeWidget::setup_connections()
{
  // global actions
  this->connect(this,
                &gngui::GraphViewer::graph_clear_request,
                this,
                &GraphNodeWidget::on_graph_clear_request);

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

  // node actions
  this->connect(this,
                &gngui::GraphViewer::connection_deleted,
                this,
                &GraphNodeWidget::on_connection_deleted);

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
}

} // namespace hesiod
