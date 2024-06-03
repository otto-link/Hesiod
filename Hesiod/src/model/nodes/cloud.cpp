/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Cloud::Cloud(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Cloud::Cloud");

  // model
  this->node_caption = "Cloud";

  // inputs
  this->input_types = {};
  this->input_captions = {};

  // outputs
  this->output_captions = {"cloud"};
  this->output_types = {CloudData().type()};

  // attributes
  this->attr["cloud"] = NEW_ATTR_CLOUD();
  this->attr_ordered_key = {"cloud"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<CloudData>();
    this->compute();
  }

  // documentation
  this->description = "Set of points.";

  this->input_descriptions = {};
  this->output_descriptions = {"Set of points (x, y) and elevations z."};

  this->attribute_descriptions["cloud"] = "Cloud data.";
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

  LOG->trace("computing node {}", this->name().toStdString());

  *this->out->get_ref() = GET_ATTR_CLOUD("cloud");

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
