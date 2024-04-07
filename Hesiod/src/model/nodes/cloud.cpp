/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Cloud::Cloud(const ModelConfig &config) : BaseNode(config)
{
  LOG_DEBUG("Cloud::Cloud");

  // model
  this->node_caption = "Cloud";

  // inputs
  this->input_types = {};
  this->input_captions = {};

  // outputs
  this->output_captions = {"cloud"};
  this->output_types = {CloudData().type()};
  this->out = std::make_shared<CloudData>();

  // attributes
  this->attr["cloud"] = NEW_ATTR_CLOUD();
  this->attr_ordered_key = {"cloud"};

  // update
  this->compute();
}

std::shared_ptr<QtNodes::NodeData> Cloud::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Cloud::setInData(std::shared_ptr<QtNodes::NodeData> /* data */,
                      QtNodes::PortIndex /* port_index */)
{
}

// --- computing

void Cloud::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  *this->out->get_ref() = GET_ATTR_CLOUD("cloud");

  // propagate
  QtNodes::PortIndex const out_port_index = 0;
  Q_EMIT this->computingFinished();
  Q_EMIT this->dataUpdated(out_port_index);
}

} // namespace hesiod
