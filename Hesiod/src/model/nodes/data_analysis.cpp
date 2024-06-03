/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

DataAnalysis::DataAnalysis(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("DataAnalysis::DataAnalysis");

  // model
  this->node_caption = "DataAnalysis";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {};
  this->output_types = {};

  // no attributes

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->compute();
  }

  // documentation
  this->description = "DataAnalysis.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {};
}

std::shared_ptr<QtNodes::NodeData> DataAnalysis::outData(
    QtNodes::PortIndex /* port_index */)
{
  // no output, return a dummy pointer
  return std::shared_ptr<QtNodes::NodeData>();
}

void DataAnalysis::setInData(std::shared_ptr<QtNodes::NodeData> data,
                             QtNodes::PortIndex /* port_index */)
{
  if (!data)
  {
    QtNodes::PortIndex const out_port_index = 0;
    Q_EMIT this->dataInvalidated(out_port_index);
  }

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void DataAnalysis::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  Q_EMIT this->computingStarted();
  // empty on purpose

  // not output, do not propagate
  Q_EMIT this->computingFinished();
}

} // namespace hesiod
