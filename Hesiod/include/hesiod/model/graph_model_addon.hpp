/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QtNodes/DataFlowGraphModel>

#include "hesiod/model/model_config.hpp"

namespace hesiod
{

class HsdDataFlowGraphModel : public QtNodes::DataFlowGraphModel
{
  Q_OBJECT

public:
  HsdDataFlowGraphModel(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry,
                        hesiod::ModelConfig *p_model_config)
      : DataFlowGraphModel(registry), p_model_config(p_model_config)
  {
    QObject::connect(this,
                     &hesiod::HsdDataFlowGraphModel::nodeCreated,
                     this,
                     &hesiod::HsdDataFlowGraphModel::onNodeAdded);
  }

Q_SIGNALS:
  void computingStarted(QtNodes::NodeId const node_id);

  void computingFinished(QtNodes::NodeId const node_id);

public Q_SLOTS:
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

  void load(QJsonObject const &jsonDocument) override // DataFlowGraphModel
  {
    this->p_model_config->load(jsonDocument["model_config"].toObject());
    DataFlowGraphModel::load(jsonDocument);
  }

  void load(QJsonObject const &jsonDocument, hesiod::ModelConfig &model_config_override)
  {
    // a configuration can be provided as an input to override the configuration provided
    // by the json document (used for batch mode for instance)
    *this->p_model_config = model_config_override;
    DataFlowGraphModel::load(jsonDocument);
  }

  QJsonObject save() const override // DataFlowGraphModel
  {
    QJsonObject sceneJson;
    sceneJson = DataFlowGraphModel::save();
    sceneJson["model_config"] = this->p_model_config->save();
    return sceneJson;
  }

private:
  hesiod::ModelConfig *p_model_config;
};
} // namespace hesiod