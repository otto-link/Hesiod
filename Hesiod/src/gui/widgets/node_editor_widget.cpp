/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QShortcut>

#include "hesiod/gui/node_editor_widget.hpp"
#include "hesiod/model/model_registry.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

NodeEditorWidget::NodeEditorWidget(std::string graph_id, QWidget *parent)
    : QWidget(parent), graph_id(graph_id)
{
  this->model_config = hesiod::ModelConfig();

  // initialize the registry
  this->model_config.compute_nodes_at_instanciation = false;
  std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry = register_data_models(
      &this->model_config);

  this->model_config.compute_nodes_at_instanciation = true;
  this->model = std::make_unique<hesiod::HsdDataFlowGraphModel>(registry,
                                                                &this->model_config,
                                                                this->graph_id);

  // build layout
  QGridLayout *layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // --- widgets

  this->scene = std::make_unique<QtNodes::DataFlowGraphicsScene>(*this->model.get());

  // viewer 3d
  this->viewer3d = new hesiod::Viewer3dWidget(&this->model_config,
                                              this->get_scene_ref(),
                                              parent,
                                              "Viewer 3D");
  this->viewer3d->hide();
  layout->addWidget(this->viewer3d, 0, 0);

  // viewer2d and settings
  {
    QWidget     *viewers = new QWidget(this);
    QVBoxLayout *layout_vert = new QVBoxLayout(viewers);

    this->node_settings_widget = new NodeSettingsWidget(this->get_scene_ref(),
                                                        parent,
                                                        "Node settings");
    this->node_settings_widget->hide();

    this->viewer2d = new hesiod::Viewer2dWidget(&this->model_config,
                                                this->get_scene_ref(),
                                                parent,
                                                "Viewer 2D");
    this->viewer2d->hide();

    layout_vert->addWidget(this->viewer2d);
    layout_vert->addWidget(this->node_settings_widget);
    layout_vert->addStretch();

    viewers->setLayout(layout_vert);
    layout->addWidget(viewers, 0, 2);
  }

  // node editor
  auto view = new QtNodes::GraphicsView(scene.get());
  layout->addWidget(view, 0, 1);

  // quick access
  QWidget     *quick_access = new QWidget(this);
  QVBoxLayout *layout_quick_access = new QVBoxLayout(quick_access);

  QPushButton *node_settings_button = new QPushButton("node settings");
  node_settings_button->setCheckable(true);
  node_settings_button->setChecked(false);
  layout_quick_access->addWidget(node_settings_button);

  QPushButton *view2d_button = new QPushButton("view 2D");
  view2d_button->setCheckable(true);
  view2d_button->setChecked(false);
  layout_quick_access->addWidget(view2d_button);

  QPushButton *view3d_button = new QPushButton("view 3D");
  view3d_button->setCheckable(true);
  view3d_button->setChecked(false);
  layout_quick_access->addWidget(view3d_button);

  QPushButton *model_config_button = new QPushButton("graph config.");
  layout_quick_access->addWidget(model_config_button);

  layout_quick_access->addStretch();
  quick_access->setLayout(layout_quick_access);

  layout->addWidget(quick_access, 0, 3);

  // add layout
  layout->setColumnStretch(1, 1);
  this->setLayout(layout);

  // --- keyboard shorcuts

  QShortcut *settings_shortcut = new QShortcut(QKeySequence("1"), this);
  QShortcut *view2d_shortcut = new QShortcut(QKeySequence("2"), this);
  QShortcut *view3d_shortcut = new QShortcut(QKeySequence("3"), this);

  // --- connections

  connect(model_config_button,
          &QPushButton::released,
          this,
          &NodeEditorWidget::update_model_configuration);

  connect(settings_shortcut,
          &QShortcut::activated,
          [this, node_settings_button]() {
            this->toggle_widget_visibility(this->node_settings_widget,
                                           node_settings_button);
          });

  connect(node_settings_button,
          &QPushButton::released,
          [this, node_settings_button]() {
            this->toggle_widget_visibility(this->node_settings_widget,
                                           node_settings_button);
          });

  connect(view2d_shortcut,
          &QShortcut::activated,
          [this, view2d_button]()
          { this->toggle_widget_visibility(this->viewer2d, view2d_button); });

  connect(view2d_button,
          &QPushButton::released,
          [this, view2d_button]()
          { this->toggle_widget_visibility(this->viewer2d, view2d_button); });

  connect(view3d_shortcut,
          &QShortcut::activated,
          [this, view3d_button, layout]()
          {
            layout->setColumnStretch(0, 1);
            this->toggle_widget_visibility(this->viewer3d, view3d_button);
            if (this->viewer3d->isVisible())
              layout->setColumnStretch(0, 1);
            else
              layout->setColumnStretch(0, 0);
          });

  connect(view3d_button,
          &QPushButton::released,
          [this, view3d_button, layout]()
          {
            this->toggle_widget_visibility(this->viewer3d, view3d_button);
            if (this->viewer3d->isVisible())
              layout->setColumnStretch(0, 1);
            else
              layout->setColumnStretch(0, 0);
          });

  connect(this->get_scene_ref(),
          &QtNodes::DataFlowGraphicsScene::sceneLoaded,
          view,
          &QtNodes::GraphicsView::centerScene);

  connect(
      this->get_scene_ref(),
      &QtNodes::DataFlowGraphicsScene::nodeContextMenu,
      [this](QtNodes::NodeId const node_id, QPointF const pos)
      {
        hesiod::BaseNode *p_node = this->get_model_ref()->delegateModel<hesiod::BaseNode>(
            node_id);
        p_node->context_menu(pos);
      });

  // pass-through for the node updates
  connect(this->get_model_ref(),
          &HsdDataFlowGraphModel::computingStarted,
          this,
          &NodeEditorWidget::computingStarted);

  connect(this->get_model_ref(),
          &HsdDataFlowGraphModel::computingFinished,
          this,
          &NodeEditorWidget::computingFinished);
}

void NodeEditorWidget::clear() { this->get_scene_ref()->clearScene(); }

void NodeEditorWidget::load(std::string filename)
{
  LOG_DEBUG("loading graph [%s], file [%s]", this->graph_id.c_str(), filename.c_str());

  QFile file(filename.c_str());

  if (!file.open(QIODevice::ReadOnly))
    return;

  this->clear();
  QByteArray const whole_file = file.readAll();
  this->get_model_ref()->load(QJsonDocument::fromJson(whole_file).object());

  Q_EMIT this->get_scene_ref()->sceneLoaded();
}

void NodeEditorWidget::save(std::string filename)
{
  LOG_DEBUG("saving graph [%s], file [%s]", this->graph_id.c_str(), filename.c_str());

  QFile file(filename.c_str());

  // if the file already exists, load the json document and append to it
  QJsonObject json_data;

  if (QFileInfo::exists(filename.c_str()))
  {
    if (!file.open(QIODevice::ReadOnly))
      return;
    QByteArray const whole_file = file.readAll();
    json_data = QJsonDocument::fromJson(whole_file).object();
    file.close();
  }

  if (file.open(QIODevice::WriteOnly))
  {
    // https://stackoverflow.com/questions/64665582
    QVariantMap map = json_data.toVariantMap();
    map.insert(this->get_model_ref()->save().toVariantMap());

    file.write(QJsonDocument(QJsonObject::fromVariantMap(map)).toJson());
  }
}

void NodeEditorWidget::toggle_widget_visibility(QWidget *widget, QPushButton *button)
{
  if (widget->isVisible())
    widget->hide();
  else
    widget->show();

  if (button)
    button->setChecked(widget->isVisible());
}

void NodeEditorWidget::update_model_configuration()
{
  // work on a copy of the model configuration before
  // apllying modifications
  ModelConfig       new_model_config = model_config;
  ModelConfigWidget model_config_editor(&new_model_config, this);
  int               ret = model_config_editor.exec();

  new_model_config.log_debug();

  if (ret)
  {
    LOG_DEBUG("updating model configuration");

    Q_EMIT this->before_updating_model_configuration(this->graph_id);

    // reset current views
    this->viewer2d->reset();
    this->viewer3d->reset();

    // serialize the scene
    QJsonObject scene_json = this->get_model_ref()->save();

    // modify the model config
    this->model_config = new_model_config;

    // clear out and reload the scene
    this->get_scene_ref()->clearScene();
    this->get_model_ref()->load(scene_json, this->model_config);
  }
}

} // namespace hesiod
