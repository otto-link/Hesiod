/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

PathResample::PathResample(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("PathResample::PathResample");

  // model
  this->node_caption = "PathResample";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {PathData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {PathData().type()};

  // attributes
  this->attr["delta"] = NEW_ATTR_FLOAT(0.01f, 0.001f, 0.2f, "%.3f");

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<PathData>();
    this->compute();
  }

  // documentation
  this->description = "PathResample resamples the path based to get (approximately) a "
                      "given distance between points.";

  this->input_descriptions = {"Input path."};
  this->output_descriptions = {"Output path."};

  this->attribute_descriptions["delta"] = "Target distance between the points.";
}

std::shared_ptr<QtNodes::NodeData> PathResample::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void PathResample::setInData(std::shared_ptr<QtNodes::NodeData> data,
                             QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<PathData>(data);

  this->compute();
}

// --- computing

void PathResample::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Path *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::Path *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    if (p_in->get_npoints() > 1)
      p_out->resample(GET_ATTR_FLOAT("delta"));

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
