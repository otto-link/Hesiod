/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Median3x3::Median3x3(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Median3x3::Median3x3");

  // model
  this->node_caption = "Median3x3";

  // inputs
  this->input_captions = {"input", "mask"};
  this->input_types = {HeightMapData().type(), HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // no attributes

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Median3x3 filter is a median filter with a 3x3 kernel used to "
                      "reduce noise while preserving edges in an image. This process "
                      "removes outliers and smooths the image without noise reduction "
                      "and feature preservation in digital images.";

  this->input_descriptions = {
      "Input heightmap.",
      "Mask defining the filtering intensity (expected in [0, 1])."};
  this->output_descriptions = {"Filtered heightmap."};
}

std::shared_ptr<QtNodes::NodeData> Median3x3::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Median3x3::setInData(std::shared_ptr<QtNodes::NodeData> data,
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
    this->mask = std::dynamic_pointer_cast<HeightMapData>(data);
  }

  this->compute();
}

// --- computing

void Median3x3::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_mask = HSD_GET_POINTER(this->mask);
    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out,
                    p_mask,
                    [](hmap::Array &x, hmap::Array *p_mask)
                    { hmap::median_3x3(x, p_mask); });

    p_out->smooth_overlap_buffers();

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
