/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

CloudRemapValues::CloudRemapValues(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("CloudRemapValues::CloudRemapValues");

  // model
  this->node_caption = "CloudRemapValues";

  // inputs
  this->input_captions = {"cloud"};
  this->input_types = {CloudData().type()};

  // outputs
  this->output_captions = {"cloud"};
  this->output_types = {CloudData().type()};

  // attributes
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<CloudData>();
    this->compute();
  }

  // documentation
  this->description = "CloudRemapValues remap the range of the cloud point values.";

  this->input_descriptions = {"Input cloud."};
  this->output_descriptions = {"Output cloud."};
}

std::shared_ptr<QtNodes::NodeData> CloudRemapValues::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void CloudRemapValues::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                 QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<CloudData>(data);

  this->compute();
}

// --- computing

void CloudRemapValues::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Cloud *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::Cloud *p_out = this->out->get_ref();

    // copy and remap the input
    *p_out = *p_in;

    p_out->remap_values(GET_ATTR_RANGE("remap_range").x, GET_ATTR_RANGE("remap_range").y);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
