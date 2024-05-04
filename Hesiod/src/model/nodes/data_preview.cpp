/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes/data_preview.hpp"


namespace hesiod
{

DataPreview::DataPreview(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("DataPreview::DataPreview");

  // model
  this->node_caption = "DataPreview";

  // inputs
  this->input_captions = {"elevation", "texture"};
  this->input_types = {HeightMapData().type(), HeightMapRGBAData().type()};

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
  this->description = "DataPreview.";

  this->input_descriptions = {"Data values for elevation preview.",
                              "Data values for color preview."};
  this->output_descriptions = {};
}

std::shared_ptr<QtNodes::NodeData> DataPreview::outData(
    QtNodes::PortIndex /* port_index */)
{
  // no output, return a dummy pointer
  return std::shared_ptr<QtNodes::NodeData>();
}

void DataPreview::setInData(std::shared_ptr<QtNodes::NodeData> data,
                            QtNodes::PortIndex                 port_index)
{
  if (!data)
  {
    QtNodes::PortIndex const out_port_index = 0;
    Q_EMIT this->dataInvalidated(out_port_index);
  }

  switch (port_index)
  {
  case 0:
    this->elevation = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->color = std::dynamic_pointer_cast<HeightMapRGBAData>(data);
  }

  this->compute();
}

// --- computing

void DataPreview::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  // empty on purpose

  // not output, do not propagate
  Q_EMIT this->computingFinished();
}

} // namespace hesiod
