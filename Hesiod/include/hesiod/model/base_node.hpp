/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QMenu>
#include <QWidgetAction>

#include <QtCore/QObject>
#include <QtNodes/NodeDelegateModel>

#include "highmap/primitives.hpp"

#include "hesiod/gui/preview.hpp"
#include "hesiod/gui/widgets.hpp"
#include "hesiod/model/attributes.hpp"
#include "hesiod/model/model_config.hpp"

#define HSD_GET_POINTER(X) X.lock() ? X.lock()->get_ref() : nullptr

namespace hesiod
{

class Preview; // forward decl.

/**
 * @brief BaseNode class, derived from the QtNodes delegate model to provide basic
 * features for the specialize model nodes.
 */
class BaseNode : public QtNodes::NodeDelegateModel
{
  Q_OBJECT

public:
  /**
   * @brief Reference to the model configuration.
   */
  const ModelConfig *p_config = nullptr;

  /**
   * @brief Constructor.
   */
  BaseNode(const ModelConfig *p_config);

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
   * @brief Dump the full description (node and inputs/outputs) of the node to an ASCII
   * file.
   */
  void full_description_to_file(std::string filename);

  /**
   * @brief Dump the full description (node and inputs/outputs) of the node to a json
   * object.
   */
  QJsonObject full_description_to_json();

  /**
   * @brief Return the full description (node and inputs/outputs) of the node.
   * @return Full description.
   */
  std::string get_full_description();

  /**
   * @brief Return the node name (same as the caption here).
   * @return Node name.
   */
  QString name() const override;

  /**
   * @brief Return the number of ports for each port type (input or output), based on
   * the input_types nd output_types storage.
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
   * @brief Return whether port captions should be visible or not (all the ports
   * trated the same way here and the method always returns true to allow different
   * caption for each port -> see portCaption).
   * @return True for visible.
   */
  bool portCaptionVisible(QtNodes::PortType /* port_type */,
                          QtNodes::PortIndex /* port_index */) const override;

  /**
   * @brief Triggers the signals 'dataUpdated' for each output, if any.
   */
  void trigger_outputs_updated();

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
   * @brief Node generic attributes, use to define/store the algorithm parameters and
   * also use to automatically generate the settings GUI.
   */
  std::map<std::string, std::unique_ptr<Attribute>> attr = {};

protected:
  /**
   * @brief Node caption
   */
  QString node_caption = "??? undefined ???";

  /**
   * brief What the node does and what for.
   */
  std::string description = "no description";

  /**
   * brief Input plain text descriptions.
   */
  std::vector<std::string> input_descriptions = {};

  /**
   * brief Output plain text descriptions.
   */
  std::vector<std::string> output_descriptions = {};

  /**
   * brief Attributes plain text descriptions.
   */
  std::map<std::string, std::string> attribute_descriptions = {};

  /**
   * @brief Input and output type storage, to facilitate and automatize some ports
   * feature definition (see @link nPorts and @link dataType).
   */
  std::vector<QtNodes::NodeDataType> input_types = {};
  std::vector<QtNodes::NodeDataType> output_types = {};

  /**
   * @brief Input and output caption storage, to facilitate and automatize some ports
   * feature definition (see @link nPorts and @link dataType)..
   */
  std::vector<QString> input_captions = {};
  std::vector<QString> output_captions = {};

  // --- SIGNALS (model and GUI)
Q_SIGNALS:

  /**
   * @brief Signal emitted when the node has been loaded.
   */
  void node_loaded(); // model

  /**
   * @brief Signal emitted when any attribute has been modified with the GUI.
   */
  void settings_changed(); // GUI

  // --- GUI

public:
  /**
   *@brief Return the reference to the NodeData for the node preview.
   */
  virtual QtNodes::NodeData *get_preview_data() { return nullptr; }

  /**
   *@brief Return the reference to the NodeData for the 2D viewer.
   */
  virtual QtNodes::NodeData *get_viewer2d_data() { return nullptr; }

  /**
   *@brief Return the reference to the elevation NodeData for the 3D viewer.
   */
  virtual QtNodes::NodeData *get_viewer3d_data() { return nullptr; }

  /**
   *@brief Return the reference to the color NodeData for the 3D viewer.
   */
  virtual QtNodes::NodeData *get_viewer3d_color() { return nullptr; }

  /**
   * @brief Defines the display order of the attributes in the settings widget.
   */
  std::vector<std::string> attr_ordered_key = {};

  /**
   * @brief Node context menu.
   */
  virtual void context_menu(const QPointF /* pos */);

  /**
   * @brief Return the reference to the widget embedded within the grpahic node.
   * @return Widget reference.
   */
  virtual QWidget *embeddedWidget() override;

private:
  /**
   * @brief Reference to the preview widget.
   */
  Preview *preview = nullptr;

  /**
   * @brief Reference to the Qt context menu widget.
   */
  QMenu *qmenu = nullptr;
};

} // namespace hesiod
