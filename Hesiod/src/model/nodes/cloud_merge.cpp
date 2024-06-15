/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

CloudMerge::CloudMerge(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("CloudMerge::CloudMerge");

  // model
  this->node_caption = "CloudMerge";

  // inputs
  this->input_captions = {"cloud1", "cloud2"};
  this->input_types = {CloudData().type(), CloudData().type()};

  // outputs
  this->output_captions = {"cloud"};
  this->output_types = {CloudData().type()};

  // attributes

  // none

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<CloudData>();
    this->compute();
  }

  // documentation
  this->description = "CloudMerge merges two clouds into a single one.";

  this->input_descriptions = {"Input cloud.", "Input cloud."};
  this->output_descriptions = {"Merged cloud."};
}

std::shared_ptr<QtNodes::NodeData> CloudMerge::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void CloudMerge::setInData(std::shared_ptr<QtNodes::NodeData> data,
                           QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->in1 = std::dynamic_pointer_cast<CloudData>(data);
    break;
  case 1:
    this->in2 = std::dynamic_pointer_cast<CloudData>(data);
  }

  this->compute();
}

// --- computing

void CloudMerge::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Cloud *p_in1 = HSD_GET_POINTER(this->in1);
  hmap::Cloud *p_in2 = HSD_GET_POINTER(this->in2);

  if (p_in1 && p_in2)
  {
    Q_EMIT this->computingStarted();

    hmap::Cloud *p_out = this->out->get_ref();

    // convert the input
    *p_out = hmap::merge_cloud(*p_in1, *p_in2);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
