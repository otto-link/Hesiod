/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ThermalRib::ThermalRib(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("ThermalRib::ThermalRib");

  // model
  this->node_caption = "ThermalRib";

  // inputs
  this->input_captions = {"input", "bedrock"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["iterations"] = NEW_ATTR_INT(50, 1, 200);

  this->attr_ordered_key = {"iterations"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "ThermalRib performs thermal erosion using a 'rib' algorithm "
                      "(taken from Geomorph)";

  this->input_descriptions = {
      "Input heightmap.",
      "Bedrock elevation, erosion process cannot carve the heightmap further down this "
      "point."};
  this->output_descriptions = {"Eroded heightmap."};

  this->attribute_descriptions["iterations"] = "Number of iterations.";
}

std::shared_ptr<QtNodes::NodeData> ThermalRib::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void ThermalRib::setInData(std::shared_ptr<QtNodes::NodeData> data,
                           QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->in = std::dynamic_pointer_cast<HeightMapData>(data);
    break;
  case 1:
    this->bedrock = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void ThermalRib::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_bedrock = HSD_GET_POINTER(this->bedrock);
    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(
        *p_out,
        p_bedrock,
        [this](hmap::Array &h_out, hmap::Array *p_bedrock_array)
        { hmap::thermal_rib(h_out, GET_ATTR_INT("iterations"), p_bedrock_array); });

    p_out->smooth_overlap_buffers();

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
