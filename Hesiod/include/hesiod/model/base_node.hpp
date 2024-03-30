/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QMenu>
#include <QSizePolicy>
#include <QWidgetAction>

#include <QtCore/QObject>
#include <QtNodes/NodeDelegateModel>

#include "highmap/primitives.hpp"
#include "macrologger.h"

#include "hesiod/gui/preview.hpp"
#include "hesiod/gui/widgets.hpp"
#include "hesiod/model/attributes.hpp"
#include "hesiod/model/model_config.hpp"

#define HSD_GET_POINTER(X) X.lock() ? X.lock()->get_ref() : nullptr

namespace hesiod
{

/**
 * @brief BaseNode class, derived from the QtNodes delegate model to provide basic
 * features for the specialize model nodes.
 */
class BaseNode : public QtNodes::NodeDelegateModel
{
  Q_OBJECT

public:
  /**
   * @brief Constructor.
   */
  BaseNode(const ModelConfig &config);

  /**
   * @brief Return the node caption.
   * @return Node caption.
   */
  QString caption() const override { return this->node_caption; }

  /**
   * @brief Return whether the node caption is visible or not.
   * @return Visible if true.
   */
  bool captionVisible() const override { return true; }

  /**
   * @brief Node computation.
   */
  virtual void compute() = 0;

  /**
   * @brief Return the data type for the requested port.
   * @param port_type Input or output.
   * @param port_index Port index.
   * @return Data type.
   */
  QtNodes::NodeDataType dataType(QtNodes::PortType  port_type,
                                 QtNodes::PortIndex port_index) const override;

  /**
   * @brief Return the node name (same as the caption here).
   * @return Node name.
   */
  QString name() const override;

  /**
   * @brief Return the number of ports for each port type (input or output), based on the
   * input_types nd output_types storage.
   * @param port_type Input or output.
   * @return Number of ports.
   */
  unsigned int nPorts(QtNodes::PortType port_type) const override;

  /**
   * @brief Return the caption of a given port, based on the input_captions and
   * output_captions storages.
   * @param port_type Input or output.
   * @param port_index Port index.
   * @return Port caption.
   */
  QString portCaption(QtNodes::PortType  port_type,
                      QtNodes::PortIndex port_index) const override;

  /**
   * @brief Return whether port captions should be visible or not (all the ports trated
   * the same way here and the method always returns true to allow different caption for
   * each port -> see portCaption).
   * @return True for visible.
   */
  bool portCaptionVisible(QtNodes::PortType /* port_type */,
                          QtNodes::PortIndex /* port_index */) const override;

  // --- serialization

  /**
   * @brief Save the "user" internal state of the node.
   * @return json object.
   */
  QJsonObject save() const override;

  /**
   * @brief Load the "user" internal state of the node.
   * @param p json object to parse.
   */
  void load(QJsonObject const &p) override;

  // --- debugging

  /**
   * @brief Display a bunch of debugging infos.
   */
  void log_debug();

  /**
   * @brief Node generic attributes, use to define/store the algorithm parameters and also
   * use to automatically generate the settings GUI.
   */
  std::map<std::string, std::unique_ptr<Attribute>> attr = {};

protected:
  /**
   * @brief Reference to the model configuration.
   */
  const ModelConfig *p_config = nullptr;

  /**
   * @brief Node caption
   */
  QString node_caption = "??? undefined ???";

  /**
   * @brief Input and output type storage, to facilitate and automatize some ports feature
   * definition (see @link nPorts and @link dataType).
   */
  std::vector<QtNodes::NodeDataType> input_types = {};
  std::vector<QtNodes::NodeDataType> output_types = {};

  /**
   * @brief Input and output caption storage, to facilitate and automatize some ports
   * feature definition (see @link nPorts and @link dataType)..
   */
  std::vector<QString> input_captions = {};
  std::vector<QString> output_captions = {};

  // --- GUI

public:
  QtNodes::NodeData *p_preview_data = nullptr;
  QtNodes::NodeData *p_viewer2d_data = nullptr;

  /**
   * @brief Defines the display order of the attributes in the settings widget.
   */
  std::vector<std::string> attr_ordered_key = {};

  /**
   * @brief Node context menu.
   */
  virtual void context_menu(const QPointF /* pos */)
  {
    if (!this->qmenu)
    {
      // menu populated here to ensure this is done by the derived class, not by the
      // current base class BaseNode (or the attribute mapping will be empty)
      AttributesWidget *attributes_widget = new AttributesWidget(&this->attr,
                                                                 &this->attr_ordered_key);
      this->qmenu = new QMenu();
      // this->qmenu.setStyleSheet("QMenu::item { padding: 5px; }");

      {
        QLabel *label = new QLabel(this->caption());
        QFont   f = label->font();
        f.setBold(true);
        label->setFont(f);
        QWidgetAction *widget_action = new QWidgetAction(qmenu);
        widget_action->setDefaultWidget(label);
        this->qmenu->addAction(widget_action);
      }

      this->qmenu->addSeparator();

      {
        QWidgetAction *widget_action = new QWidgetAction(qmenu);
        widget_action->setDefaultWidget(attributes_widget);
        this->qmenu->addAction(widget_action);
      }

      connect(attributes_widget,
              &AttributesWidget::value_changed,
              [this]() { this->compute(); });
    }

    this->qmenu->popup(QCursor::pos());
  }

  /**
   * @brief Return the reference to the widget embedded within the grpahic node.
   * @return Widget reference.
   */
  QWidget *embeddedWidget() override
  {
    if (this->p_preview_data && !this->preview)
    {
      this->preview = new Preview(this->p_config, this->p_preview_data);
      connect(this,
              &NodeDelegateModel::dataUpdated,
              this->preview,
              &Preview::update_image,
              Qt::UniqueConnection);
    }
    return (QWidget *)this->preview;
  }

private:
  Preview *preview = nullptr;
  QMenu   *qmenu = nullptr;
};

} // namespace hesiod
