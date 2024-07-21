/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ShiftElevation::ShiftElevation(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("ShiftElevation::ShiftElevation");

  // model
  this->node_caption = "ShiftElevation";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["shift"] = NEW_ATTR_FLOAT(0.f, -2.f, 2.f, "%.2f");
  this->attr_ordered_key = {"shift"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "The ShiftElevation operator involves adjusting each data point by "
                      "adding it with a predetermined constant.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Output heightmap."};

  this->attribute_descriptions
      ["shift"] = "Constant by which each elevation values will be added.";
}

std::shared_ptr<QtNodes::NodeData> ShiftElevation::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void ShiftElevation::setInData(std::shared_ptr<QtNodes::NodeData> data,
                               QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void ShiftElevation::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out, [this](hmap::Array &x) { x += GET_ATTR_FLOAT("shift"); });

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
