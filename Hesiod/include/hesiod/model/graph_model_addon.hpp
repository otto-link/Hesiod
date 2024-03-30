/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QtNodes/DataFlowGraphModel>

namespace hesiod
{

class HsdDataFlowGraphModel : public QtNodes::DataFlowGraphModel
{
  Q_OBJECT

public:
  HsdDataFlowGraphModel(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry)
      : DataFlowGraphModel(registry)
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
};
} // namespace hesiod