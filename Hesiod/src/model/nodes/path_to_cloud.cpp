/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

PathToCloud::PathToCloud(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("PathToCloud::PathToCloud");

  // model
  this->node_caption = "PathToCloud";

  // inputs
  this->input_captions = {"path"};
  this->input_types = {PathData().type()};

  // outputs
  this->output_captions = {"cloud"};
  this->output_types = {CloudData().type()};

  // no attributes

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<CloudData>();
    this->compute();
  }

  // documentation
  this->description = "PathToCloud convert a Path to a set of points (Cloud).";

  this->input_descriptions = {"Input path."};
  this->output_descriptions = {"Output cloud."};
}

std::shared_ptr<QtNodes::NodeData> PathToCloud::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void PathToCloud::setInData(std::shared_ptr<QtNodes::NodeData> data,
                            QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<PathData>(data);

  this->compute();
}

// --- computing

void PathToCloud::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::Path *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::Cloud *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = hmap::Cloud(p_in->points);

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
