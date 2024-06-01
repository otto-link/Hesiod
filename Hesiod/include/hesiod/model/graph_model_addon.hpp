/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QtNodes/DataFlowGraphModel>

#include "hesiod/model/model_config.hpp"

namespace hesiod
{

/**
 * @brief HsdDataFlowGraphModel, derived from the DataFlowGraphModel provided by QtNodes.
 */
class HsdDataFlowGraphModel : public QtNodes::DataFlowGraphModel
{
  Q_OBJECT

public:
  /**
   * @brief Graph Id.
   */
  std::string graph_id;

  /**
   * @brief Constructor.
   * @param registry
   * @param p_model_config
   */
  HsdDataFlowGraphModel(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry,
                        hesiod::ModelConfig *p_model_config,
                        std::string          graph_id = "default_id")
      : DataFlowGraphModel(registry), graph_id(graph_id), p_model_config(p_model_config)
  {
    QObject::connect(this,
                     &hesiod::HsdDataFlowGraphModel::nodeCreated,
                     this,
                     &hesiod::HsdDataFlowGraphModel::onNodeAdded);
  }

Q_SIGNALS:
  /**
   * @brief Node computing has started.
   * @param node_id Node id.
   */
  void computingStarted(QtNodes::NodeId const node_id);

  /**
   * @brief Node is finished (emitted before the out signals).
   * @param node_id Node id.
   */
  void computingFinished(QtNodes::NodeId const node_id);

public Q_SLOTS:
  /**
   * @brief Called when a node is added (performs some connections before the model and
   * the node signals).
   * @param node_id Node id.
   */
  void onNodeAdded(QtNodes::NodeId const node_id)
  {
    QtNodes::NodeDelegateModel *p_node = this->delegateModel<QtNodes::NodeDelegateModel>(
        node_id);

    connect(p_node,
            &QtNodes::NodeDelegateModel::computingFinished,
            [this, node_id]() { Q_EMIT this->computingFinished(node_id); });

    connect(p_node,
            &QtNodes::NodeDelegateModel::computingStarted,
            [this, node_id]() { Q_EMIT this->computingStarted(node_id); });
  }

  /**
   * @brief Model serialization (load).
   * @param jsonDocument Json doc.
   */
  void load(QJsonObject const &jsonDocument) override // DataFlowGraphModel
  {
    QJsonObject current_graph_json = jsonDocument[this->graph_id.c_str()].toObject();

    this->p_model_config->load(current_graph_json["model_config"].toObject());
    DataFlowGraphModel::load(current_graph_json);
  }

  /**
   * @brief Model serialization (load).
   * @param jsonDocument Json doc
   * @param model_config_override Reference to a model configuration used to override the
   * model configuration provided by the json document.
   */
  void load(QJsonObject const &jsonDocument, hesiod::ModelConfig &model_config_override)
  {
    // a configuration can be provided as an input to override the configuration provided
    // by the json document (used for batch mode for instance)
    *this->p_model_config = model_config_override;
    DataFlowGraphModel::load(jsonDocument[this->graph_id.c_str()].toObject());
  }

  /**
   * @brief Model serialization (save).
   * @return Json object.
   */
  QJsonObject save() const override // DataFlowGraphModel
  {
    QJsonObject graph_json;
    graph_json = DataFlowGraphModel::save();
    graph_json["model_config"] = this->p_model_config->save();

    graph_json["hesiod_version_major"] = QString::number(HESIOD_VERSION_MAJOR);
    graph_json["hesiod_version_minor"] = QString::number(HESIOD_VERSION_MINOR);
    graph_json["hesiod_version_patch"] = QString::number(HESIOD_VERSION_PATCH);

    QJsonObject full_json;
    full_json[this->graph_id.c_str()] = graph_json;

    return full_json;
  }

private:
  /**
   * @brief Reference to the model configuration.
   */
  hesiod::ModelConfig *p_model_config;
};

// --- helpers

QtNodes::NodeId add_graph_example(hesiod::HsdDataFlowGraphModel *p_model,
                                  std::string                    node_type,
                                  std::string                    category = "");

} // namespace hesiod